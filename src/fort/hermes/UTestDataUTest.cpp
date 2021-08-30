#include "UTestDataUTest.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <fort/hermes/Header.pb.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>

namespace fort {
namespace hermes {

std::unique_ptr<UTestData> UTestData::s_instance;

const UTestData & UTestData::Instance() {
	if ( !s_instance ) {
		s_instance = std::make_unique<UTestData>(TempDirPath());
	}
	return *s_instance;
}

void UTestData::Release() {
	s_instance.reset();
}

std::filesystem::path UTestData::TempDirPath() {
	std::ostringstream oss;
	oss << "fort-hermes-tests-" << getpid();
	return std::filesystem::temp_directory_path() / oss.str();
}

UTestData::UTestData(const std::filesystem::path & basepath) {
	WriteSequenceInfos(basepath);
	d_basepath = basepath;
}

UTestData::~UTestData() {
	if ( d_basepath.empty() ) {
		return;
	}
	std::filesystem::remove_all(d_basepath);
}

const std::filesystem::path & UTestData::Basepath() const {
	return d_basepath;
}

const UTestData::SequenceInfo & UTestData::NormalSequence() const {
	return d_normal;
}

const UTestData::SequenceInfo & UTestData::TruncatedClassicSequence() const {
	return d_truncated;
}

const UTestData::SequenceInfo & UTestData::TruncatedDualStreamSequence() const {
	return d_truncatedDual;
}

const UTestData::SequenceInfo & UTestData::NoFooter() const {
	return d_noFooter;
}

const UTestData::SequenceInfo & UTestData::NoHeader() const {
	return d_noHeader;
}

struct SequenceInfoWriteArgs {
	std::filesystem::path Basepath;
	std::string           Basename;
	FrameReadout          Readout;
	size_t                NumberOfSegments;
	size_t                ReadoutsPerSegment;
	bool                  MissFooter;
	bool                  Dual;
	bool                  Truncated;
	bool                  NoHeader;
};


std::string HermesFileName(const std::string & basename,
                           size_t index) {
	std::ostringstream oss;
	oss << basename << "." << std::setw(4) << std::setfill('0') << index << ".hermes";
	return oss.str();
}


void TruncateFile(const std::filesystem::path & filepath, int bytes) {
	FILE * file = fopen(filepath.c_str(),"r+");
	if ( file == nullptr ) {
		throw std::runtime_error("open('"
		                         + filepath.string()
		                         + "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY): "
		                         + std::to_string(errno));
	}
	if ( fseeko(file,-bytes,SEEK_END) != 0 ) {
		throw std::runtime_error("fseeko('"
		                         + filepath.string()
		                         + "',"
		                         + std::to_string(-bytes)
		                         + ",SEEK_END): "
		                         + std::to_string(errno));
	}
	auto offset = ftello(file);
	if ( ftruncate(fileno(file),offset) != 0 ) {
				throw std::runtime_error("ftruncate('" + filepath.string()
				                         + "',"
				                         + std::to_string(offset)
				                         + "): "
				                         + std::to_string(errno));
	}

	fclose(file);
}

size_t WriteSegment(UTestData::SequenceInfo & info,
                    const SequenceInfoWriteArgs & args,
                    size_t i,
                    size_t frameID) {
	auto filepath = args.Basepath / HermesFileName(args.Basename,i);
	Header header;
	auto version = header.mutable_version();
	version->set_vmajor(0);
	version->set_vminor(1);
	header.set_type(Header::Type::Header_Type_File);
	header.set_width(args.Readout.width());
	header.set_height(args.Readout.height());
	int fd = open(filepath.c_str(),
	              O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	              0644);

	if ( fd <= 0 ) {
		throw std::runtime_error("open('" + filepath.string() + "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY): " + std::to_string(errno));
	}
	auto  file = std::make_unique<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gziped = std::make_unique<google::protobuf::io::GzipOutputStream>(file.get());
	std::unique_ptr<google::protobuf::io::FileOutputStream> uncomp;
	if ( args.Truncated && args.Dual && i == (args.NumberOfSegments - 1 ) ) {
		int fdUncomp = open((filepath.string() + "unc").c_str(),
		                    O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
		                    0644);
		if ( fd <= 0 ) {
			throw std::runtime_error("open('" + filepath.string() + "unc',O_CREAT | O_TRUNC | O_RDWR | O_BINARY): " + std::to_string(errno));
		}
		uncomp = std::make_unique<google::protobuf::io::FileOutputStream>(fdUncomp);
		uncomp->SetCloseOnDelete(true);
	}
	auto write =
		[&gziped,&uncomp] ( const google::protobuf::MessageLite & m) {
			if ( google::protobuf::util::SerializeDelimitedToZeroCopyStream(m,gziped.get()) == false ) {
				throw std::runtime_error("Could not write message");
			}
			if (uncomp && google::protobuf::util::SerializeDelimitedToZeroCopyStream(m,uncomp.get()) == false ) {
				throw std::runtime_error("Could not write uncompressed message");
			}
		};

	if ( args.NoHeader == false ) {
		std:: cerr << "Printing header for " << filepath <<  std::endl;
		write(header);
	} else {
		std:: cerr << "Not printing header for " << filepath <<  std::endl;
	}

	FileLine line;

	for ( size_t j = 0; j < args.ReadoutsPerSegment; ++j) {
		FrameReadout ro;
		uint64_t ts = (frameID - args.Readout.frameid()) * 1e5;
		ro.set_frameid(frameID);
		ro.set_timestamp(ts + 1);
		auto t = ro.mutable_time();
		int64_t ts_s = ts / 1e9;
		int64_t ts_ns = ts - ts_s + args.Readout.time().nanos();
		while( ts_ns >= 1e9 ) {
			ts_s += 1;
			ts_ns -= 1e9;
		}
		t->set_seconds(args.Readout.time().seconds() + ts_s);
		t->set_nanos(ts_ns);
		for ( const auto & t : args.Readout.tags() ) {
			auto tag = ro.add_tags();
			tag->set_id(t.id());
			tag->set_x(t.x());
			tag->set_y(t.y());
			tag->set_theta(t.theta());
		}
		line.set_allocated_readout(&ro);
		write(line);
		line.release_readout();

		ro.set_width(args.Readout.width());
		ro.set_height(args.Readout.height());

		info.Readouts.push_back(ro);

		++frameID;
	}



	if ( i < (args.NumberOfSegments -1) ||
	     ( args.MissFooter == false && args.Truncated == false ) ) {
		auto footer = line.mutable_footer();
		if ( i < args.NumberOfSegments - 1 ) {
			footer->set_next(HermesFileName(args.Basename,i+1));
		}
		write(line);
	}

	if ( (i == (args.NumberOfSegments - 1) ) && args.Truncated == true ) {
		gziped.reset();
		uncomp.reset();
		file.reset();
		TruncateFile(filepath,60);
		if ( args.Dual == true ) {
			TruncateFile(filepath.string() + "unc",60);
		}
	}

	info.Segments.push_back(filepath);
	return frameID;

}

UTestData::SequenceInfo WriteSequenceInfo(const SequenceInfoWriteArgs & args) {
	UTestData::SequenceInfo res;
	std::filesystem::create_directories(args.Basepath);

	size_t frameID = args.Readout.frameid();
	for ( size_t i = 0; i < args.NumberOfSegments; ++i) {
		frameID = WriteSegment(res,args,i,frameID);
	}
	res.ReadoutsPerSegment = args.ReadoutsPerSegment;
	return res;
}

void UTestData::WriteSequenceInfos(const std::filesystem::path & basepath) {

	FrameReadout ro;
	ro.set_width(1000);
	ro.set_height(1000);
	ro.set_frameid(1234);
	auto t = ro.add_tags();
	t->set_id(123);
	t->set_x(500);
	t->set_y(500);
	t->set_theta(3.14159265/4);


	d_normal= WriteSequenceInfo({.Basepath = basepath,
	                             .Basename = "normal",
	                             .Readout = ro,
	                             .NumberOfSegments = 3,
	                             .ReadoutsPerSegment = 10,
	                             .MissFooter = false,
	                             .Dual = false,
	                             .Truncated = false,
	                             .NoHeader = false,
		});

	d_noFooter= WriteSequenceInfo({.Basepath = basepath,
	                               .Basename = "no-footer",
	                               .Readout = ro,
	                               .NumberOfSegments = 3,
	                               .ReadoutsPerSegment = 10,
	                               .MissFooter = true,
	                               .Dual = false,
	                               .Truncated = false,
	                               .NoHeader = false,
		});

	d_truncated= WriteSequenceInfo({.Basepath = basepath,
	                                .Basename = "truncated-classic",
	                                .Readout = ro,
	                                .NumberOfSegments = 3,
	                                .ReadoutsPerSegment = 10,
	                                .MissFooter = false,
	                                .Dual = false,
	                                .Truncated = true,
	                                .NoHeader = false,
		});

	d_truncatedDual = WriteSequenceInfo({.Basepath = basepath,
	                                     .Basename = "truncated-dual",
	                                     .Readout = ro,
	                                     .NumberOfSegments = 3,
	                                     .ReadoutsPerSegment = 10,
	                                     .MissFooter = false,
	                                     .Dual = true,
	                                     .Truncated = true,
	                                     .NoHeader = false,
		});

	d_noHeader = WriteSequenceInfo({.Basepath = basepath,
	                                .Basename = "no-header",
	                                .Readout = ro,
	                                .NumberOfSegments = 1,
	                                .ReadoutsPerSegment = 2,
	                                .MissFooter = false,
	                                .Dual = false,
	                                .Truncated = false,
	                                .NoHeader = true,
		});

}


} // namespace hermes
} // namespace fort

#define failure_helper(aExpr,bExpr,a,b,field) \
	::testing::AssertionFailure() \
	<< "Value of: " << aExpr << "." << #field << std::endl \
	<< "  Actual: " << a.field << std::endl \
	<< "Expected: " << bExpr << "." << #field << std::endl \
	<< "Which is: " << b.field

#define assert_equal(aExpr,bExpr,a,b,field) do {	  \
		if ( a.field != b.field ) { \
			return failure_helper(aExpr,bExpr,a,b,field); \
		} \
	} while(0)


::testing::AssertionResult AssertReadoutEqual(const char * aExpr,
                                              const char * bExpr,
                                              const fort::hermes::FrameReadout & a,
                                              const fort::hermes::FrameReadout & b) {

	assert_equal(aExpr,bExpr,a,b,timestamp());
	assert_equal(aExpr,bExpr,a,b,frameid());
	assert_equal(aExpr,bExpr,a,b,producer_uuid());
	assert_equal(aExpr,bExpr,a,b,quads());
	assert_equal(aExpr,bExpr,a,b,width());
	assert_equal(aExpr,bExpr,a,b,height());
	assert_equal(aExpr,bExpr,a,b,tags_size());

	for ( size_t i = 0; i < b.tags_size(); ++i ) {
		assert_equal(aExpr,bExpr,a,b,tags(i).id());
		assert_equal(aExpr,bExpr,a,b,tags(i).x());
		assert_equal(aExpr,bExpr,a,b,tags(i).y());
		assert_equal(aExpr,bExpr,a,b,tags(i).theta());
	}


	return ::testing::AssertionSuccess();
}
