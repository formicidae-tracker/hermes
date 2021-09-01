from py_fort_hermes import FrameReadout

class UTestData:
    class SequenceInfo:
        def __init__(self):
            self.Segments = []
            self.Readouts = []

    def __del(self):
        shutils.rmtree(self.Basepath)

    def __init__(self):
        self.Basepath = tempfile.mkdtemp()
        self.initSequences()

    def initSequences(self):
        ro = FrameReadout()
        self.Normal = UTestData.WriteSequence(basepath = self.Basepath,
                                              basename = "normal",
                                              readout = ro,
                                              numberOfSegments = 3,
                                              readoutsPerSegment = 10,
                                              missFooter = False,
                                              dual = False,
                                              truncated = False,
                                              noHeader = False)

        self.NoFooter = UTestData.WriteSequence(basepath = self.Basepath,
                                                basename = "no-footer",
                                                readout = ro,
                                                numberOfSegments = 3,
                                                readoutsPerSegment = 10,
                                                missFooter = True,
                                                dual = False,
                                                truncated = False,
                                                noHeader = False)

        self.NoFooter = UTestData.WriteSequence(basepath = self.Basepath,
                                                basename = "no-footer",
                                                readout = ro,
                                                numberOfSegments = 1,
                                                readoutsPerSegment = 2,
                                                missFooter = False,
                                                dual = False,
                                                truncated = False,
                                                noHeader = True)

        self.Truncated = UTestData.WriteSequence(basepath = self.Basepath,
                                                 basename = "truncated",
                                                 readout = ro,
                                                 numberOfSegments = 3,
                                                 readoutsPerSegment = 10,
                                                 missFooter = False,
                                                 dual = False,
                                                 truncated = True,
                                                 noHeader = False)

        self.TruncatedDual = UTestData.WriteSequence(basepath = self.Basepath,
                                                     basename = "truncated",
                                                     readout = ro,
                                                     numberOfSegments = 3,
                                                     readoutsPerSegment = 10,
                                                     missFooter = False,
                                                     dual = True,
                                                     truncated = True,
                                                     noHeader = False)


        def WriteSequenceInfo(basepath,
                              basename,
                              readout,
                              numberOfSegments,
                              readoutsPerSegment,
                              missFooter,
                              dual,
                              truncated,
                              noHeader):
            res.Segments = [];
            res.Readouts = [];
            frameID = readout.frameID
            for i in range(numberOfSegments):
                frameID,segmentPath,readouts = UTestData.WriteSegment(basename,index = i,frameID,numberOfSegments,readoutsPerSegment,missFooter,dual,truncated,noHeader)
                res.Readouts += readouts
                res.Segments.append(segmentPath)

        def WriteSegment(basename,
                         index,
                         frameID,
                         numberOfSegments,
                         readoutsPerSegment,
                         missFooter,
                         dual,
                         truncated,
                         noHeader):
            filepath = os.path.join(basename,UTestData.HermesFileName(basename,index))

            f = gzip.open(filepath,"wb")
            write = lambda m: UTestData.WriteMessage(f,m)
            if index == numberOfSegments - 1:
                uncomp = open(filepath + "unc","wb")
                write = (lambda m:
                         UTestData.WriteMessag(f,m)
                         UTestData.WriteMessag(uncom,m))
            if noHeader == False:
                h = py_fort_hermes.Header()
                h.version.vmajor = 0
                h.version.vminor = 1
                h.type = Hermes_Type_File
                h.width = ro.width
                h.height = ro.height
                write(h)

            for i in range(readoutsPerSegment):
                ro = py_fort_hermes.FrameReadout
                ro.frameID = frameID
                ts = (frameID - readout.frameID) * 1e5
                ro.timestamp = ts + 1
                ts_s = ts / 1e9
                ts_ns = ts - ts_s + readout.time.nanos
                while ts_na
                to.time.seconds = readout.time.seconds + ts_s
