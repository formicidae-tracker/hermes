#include "FileSequence.hpp"
#include <fstream>
#include <regex>

#ifndef NDEBUG
#include <iostream>
#endif

namespace fort {
namespace hermes {
namespace details {

FileSequence::FileSequence(const std::filesystem::path &path)
    : d_directory{std::filesystem::absolute(path.parent_path())} {
	static std::regex tddFileRx{"tracking.[0-9]{4}.hermes"};
	if (!std::regex_search(path.filename().string(), tddFileRx)) {
#ifndef NDEBUG
		std::cerr << "libfort-hermes [INFO]: using empty index as " << path
		          << " does not match regex" << std::endl;
#endif
		return;
	}

	try {
		BuildFromIndexFile();
		return;
	} catch (const std::exception &e) {
#ifndef NDEBUG
		std::cerr
		    << "libfort-hermes [ERROR]: could not build file sequence for "
		    << path << " from index file: " << e.what() << std::endl;
#endif
	}

	try {
		ListFromDirectory();
	} catch (const std::exception &e) {
#ifndef NDEBUG
		std::cerr << "libfort-hermes [INFO]: could not build file sequence for "
		          << path << " from list of files: " << e.what() << std::endl;
#endif
	}
}

const SegmentInfo &FileSequence::At(const std::filesystem::path &path) const {
	return d_segments.at(std::filesystem::absolute(path));
}

void FileSequence::UpdateSegment(
    const std::filesystem::path &path,
    size_t                       currentLineIndex,
    size_t                       currentFrameNumber
) {

	if (std::filesystem::absolute(path.parent_path()) != d_directory) {
		return;
	}
	auto absPath = std::filesystem::absolute(path);
	if (d_segments.count(absPath) == 0) {
		d_needUpdate        = true;
		d_segments[absPath] = {
		    .Name      = path.filename(),
		    .Index     = 0,
		    .LineCount = 0,
		    .Start     = 0,
		    .End       = 0,
		};

		size_t index = 0;
		for (auto &[p, infos] : d_segments) {
			infos.Index = index++;
		}
	}

	auto &infos = d_segments.at(absPath);
	if (infos.LineCount < (currentLineIndex + 1)) {
		infos.LineCount = currentLineIndex + 1;
		d_needUpdate    = true;
	}
	if (currentLineIndex == 0) {
		infos.Start  = currentFrameNumber;
		d_needUpdate = true;
	}
	if (infos.End < currentFrameNumber + 1) {
		infos.End    = currentFrameNumber + 1;
		d_needUpdate = true;
	}
}

const std::filesystem::path &FileSequence::Directory() const {
	return d_directory;
}

void FileSequence::Persist() {
	if (d_needUpdate == false) {
		return;
	}
	try {
		std::ofstream file(IndexFilePath());
		file << "#Name,Count,Start,End" << std::endl;
		{
			for (const auto &[absPath, infos] : d_segments) {
				file << "\"" << infos.Name << "\"," //
				     << infos.LineCount << ","      //
				     << infos.Start << ","          //
				     << infos.End                   //
				     << std::endl;
			}
		}
	} catch (const std::exception &e) {
#ifndef NDEBUG
		std::cerr << "libfort-hermes [WARN]: could not persist index file in "
		          << d_directory << ": " << e.what() << std::endl;
#endif
		return;
	}
	d_needUpdate = false;
}

FileLineContext FileSequence::FileLineContext(
    const std::filesystem::path &path, size_t lineIndex, size_t previousFrame
) {

	try {
		const auto &infos = At(path);
		return fort::hermes::FileLineContext{
		    .Filename        = path.filename(),
		    .Directory       = d_directory.filename(),
		    .PreviousFrameID = previousFrame,
		    .SegmentIndex    = infos.Index,
		    .SegmentCount    = d_segments.size(),
		    .LineIndex       = lineIndex,
		    .LineCount       = infos.LineCount,
		};
	} catch (const std::exception &) {
		return fort::hermes::FileLineContext{
		    .Filename        = path.filename(),
		    .Directory       = d_directory.filename(),
		    .PreviousFrameID = previousFrame,
		    .SegmentCount    = d_segments.size(),
		    .LineIndex       = lineIndex,
		};
	}
}

const std::string FileSequence::SegmentIndexFilename{".segment-index.hermes"};

std::filesystem::path FileSequence::IndexFilePath() const {
	return d_directory / SegmentIndexFilename;
}

void FileSequence::BuildFromIndexFile() {
	std::ifstream indexFile(IndexFilePath());

	if (!indexFile) {
		throw std::runtime_error{
		    "\"" + IndexFilePath().string() + "\" does not exist"};
	}

	std::string line;
	size_t      index{0};
	while (std::getline(indexFile, line)) {
		if (line[0] == '#') {
			continue;
		}
		SegmentInfo infos;

		auto delim = line.find("\",");
		if (delim == std::string::npos) {
			throw std::runtime_error{"could not find '\",' in " + line};
		}
		infos.Name = line.substr(1, delim - 1);
		line       = line.substr(delim + 2);

		delim = line.find(",");
		if (delim == std::string::npos) {
			throw std::runtime_error{"could not find ',' in " + line};
		}

		infos.LineCount = (size_t)std::stol(line.substr(0, delim));
		line            = line.substr(delim + 1);

		delim = line.find(",");
		if (delim == std::string::npos) {
			throw std::runtime_error{"could not find ',' in " + line};
		}
		infos.Start = (size_t)std::stol(line.substr(0, delim));
		infos.End   = (size_t)std::stol(line.substr(delim + 1));
		infos.Index = index++;
		d_segments[std::filesystem::absolute(d_directory / infos.Name)] = infos;
	}
}

void FileSequence::ListFromDirectory() {
	static std::regex trackingFile{R"-(tracking\.([0-9]{4})\.hermes)-"};
	for (const auto &entry : std::filesystem::directory_iterator{d_directory}) {
		std::string filename = entry.path().filename();
		std::smatch match;
		if (!std::regex_match(filename, match, trackingFile)) {
			continue;
		}
		d_segments[std::filesystem::absolute(entry.path())] = {
		    .Name      = filename,
		    .Index     = (size_t)std::stol(match[1]),
		    .LineCount = 0,
		    .Start     = 0,
		    .End       = 0,
		};
	}
} // namespace details

const std::map<std::string, SegmentInfo> &
FileSequence::Segments() const noexcept {
	return d_segments;
}

} // namespace details
} // namespace hermes
} // namespace fort
