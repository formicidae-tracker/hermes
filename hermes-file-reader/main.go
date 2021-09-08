package main

import (
	"compress/gzip"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"path/filepath"

	"github.com/formicidae-tracker/hermes/src/go/hermes"
	"github.com/jessevdk/go-flags"
)

type Opts struct {
	CSVStyle     bool `long:"csv" Description:"output data in CSV"`
	ARTagConvert bool `long:"convert-artag" Description:"Convert tags to ARTAg expected ID"`
}

var ARTagID100 []int = []int{4, 57, 260, 5, 52, 59, 65, 244, 397, 540, 929, 2, 6, 7, 9, 14,
	15, 19, 24, 25, 26, 30, 38, 60, 64, 72, 73, 74, 76, 99, 102, 148,
	158, 161, 191, 192, 196, 202, 209, 227, 237, 245, 252, 268, 278, 308, 311, 329,
	518, 534, 604, 620, 646, 673, 740, 838, 954, 0, 1, 12, 13, 17, 20, 23,
	27, 28, 29, 32, 35, 40, 42, 43, 44, 48, 49, 50, 51, 53, 54, 55,
	62, 63, 66, 68, 70, 71, 80, 81, 82, 85, 91, 96, 97, 98, 100, 105,
	109, 111, 112, 113, 114, 115, 117, 118, 120, 124, 126, 127, 131, 135, 137, 139,
	140, 142, 144, 147, 149, 153, 156, 159, 169, 170, 172, 173, 174, 175, 176, 178,
	186, 190, 194, 195, 198, 206, 207, 210, 212, 215, 217, 218, 219, 220, 221, 226,
	229, 232, 238, 242, 243, 250, 251, 255, 256, 257, 261, 262, 263, 264, 265, 289,
	294, 296, 298, 299, 301, 305, 313, 318, 320, 324, 326, 331, 332, 334, 342, 347,
	353, 356, 376, 380, 387, 390, 391, 394, 395, 398, 400, 433, 437, 458, 462, 475,
	476, 482, 492, 501, 502, 507, 509, 527, 530, 538, 549, 552, 556, 560, 564, 575,
	576, 580, 592, 593, 599, 600, 607, 610, 614, 621, 632, 635, 638, 647, 650, 663,
	665, 667, 668, 669, 675, 676, 684, 697, 701, 710, 713, 722, 731, 734, 735, 743,
	744, 759, 789, 806, 807, 817, 822, 846, 848, 865, 869, 873, 898, 903, 917, 926,
	953, 964, 974, 977, 983, 990, 992, 1001, 1010, 1017, 1018, 1021, 3, 8, 10, 11,
	16, 21, 31, 33, 34, 37, 39, 45, 47, 56, 61, 67, 69, 75, 77, 78,
	79, 83, 84, 86, 88, 89, 90, 92, 93, 94, 95, 101, 103, 104, 106, 107,
	108, 110, 116, 119, 122, 123, 125, 128, 129, 130, 132, 134, 136, 138, 141, 143,
	145, 146, 150, 151, 152, 154, 155, 157, 160, 163, 164, 165, 166, 167, 168, 171,
	177, 180, 184, 188, 193, 197, 199, 200, 201, 204, 205, 216, 222, 224, 230, 231,
	233, 234, 235, 236, 239, 240, 241, 246, 248, 249, 253, 254, 258, 259, 266, 267,
	269, 271, 272, 273, 274, 275, 276, 277, 279, 280, 281, 282, 283, 285, 286, 288,
	290, 291, 292, 293, 297, 300, 303, 304, 306, 307, 309, 312, 315, 317, 323, 325,
	327, 328, 330, 333, 335, 336, 337, 338, 339, 340, 343, 344, 346, 348, 350, 352,
	354, 358, 359, 360, 361, 362, 363, 364, 366, 367, 368, 369, 370, 371, 372, 373,
	374, 375, 378, 379, 381, 382, 383, 385, 389, 392, 393, 396, 399, 401, 403, 404,
	405, 406, 408, 409, 410, 412, 413, 416, 417, 418, 419, 420, 421, 423, 424, 426,
	428, 429, 430, 432, 434, 435, 436, 441, 442, 443, 444, 446, 447, 448, 449, 450,
	451, 453, 454, 456, 457, 459, 460, 461, 464, 465, 466, 467, 468, 469, 470, 471,
	472, 473, 474, 477, 478, 480, 483, 484, 486, 487, 489, 490, 491, 494, 495, 496,
	497, 498, 500, 503, 504, 505, 506, 508, 510, 511, 512, 513, 514, 515, 516, 517,
	519, 521, 523, 524, 525, 526, 528, 531, 532, 533, 535, 536, 537, 539, 542, 543,
	544, 545, 546, 547, 548, 550, 551, 553, 554, 555, 557, 558, 559, 561, 562, 563,
	565, 567, 569, 570, 571, 572, 573, 574, 578, 581, 583, 584, 585, 586, 587, 588,
	589, 590, 591, 595, 601, 602, 603, 605, 606, 608, 609, 611, 612, 616, 618, 619,
	623, 624, 625, 626, 628, 633, 634, 636, 639, 640, 642, 644, 645, 649, 652, 657,
	658, 659, 660, 661, 662, 666, 670, 671, 672, 674, 678, 680, 681, 683, 685, 687,
	690, 691, 693, 694, 696, 698, 700, 702, 703, 704, 705, 707, 708, 709, 715, 716,
	717, 718, 719, 720, 721, 723, 725, 726, 728, 729, 730, 732, 737, 738, 739, 741,
	742, 745, 746, 747, 748, 751, 754, 755, 756, 757, 758, 760, 762, 763, 764, 765,
	766, 767, 769, 771, 773, 776, 778, 779, 782, 783, 785, 787, 788, 790, 792, 793,
	796, 798, 799, 800, 801, 803, 804, 809, 810, 811, 812, 814, 815, 818, 819, 820,
	823, 824, 825, 826, 829, 831, 834, 835, 836, 839, 840, 842, 845, 849, 852, 854,
	855, 858, 859, 860, 861, 862, 864, 866, 867, 870, 872, 874, 875, 876, 877, 878,
	879, 881, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 896, 897,
	899, 900, 901, 902, 905, 906, 908, 909, 911, 912, 913, 914, 916, 918, 919, 920,
	922, 923, 924, 925, 927, 928, 930, 931, 933, 934, 935, 936, 937, 939, 940, 941,
	944, 945, 946, 947, 949, 950, 951, 952, 956, 957, 958, 959, 960, 961, 962, 968,
	969, 971, 972, 973, 978, 979, 980, 981, 982, 984, 986, 987, 988, 991, 995, 996,
	998, 1000, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1011, 1012, 1013, 1014, 1015, 1016,
	1019, 1023, 18, 22, 36, 41, 46, 58, 87, 121, 133, 162, 179, 181, 183, 185,
	187, 189, 203, 208, 211, 213, 214, 223, 225, 228, 284, 287, 295, 302, 310, 314,
	316, 319, 321, 322, 341, 345, 349, 351, 355, 357, 365, 386, 388, 402, 407, 411,
	414, 415, 422, 425, 427, 431, 438, 439, 440, 445, 452, 455, 463, 479, 481, 485,
	488, 493, 499, 520, 522, 529, 541, 566, 568, 577, 579, 582, 594, 596, 597, 598,
	613, 615, 617, 622, 627, 629, 630, 631, 637, 641, 651, 653, 654, 656, 664, 677,
	679, 688, 689, 695, 699, 706, 711, 712, 714, 724, 727, 733, 749, 750, 752, 753,
	761, 768, 770, 772, 774, 775, 777, 780, 781, 784, 786, 794, 795, 797, 802, 805,
	808, 813, 816, 821, 827, 830, 832, 833, 837, 841, 843, 844, 847, 850, 851, 853,
	856, 857, 863, 868, 871, 880, 882, 895, 904, 907, 910, 915, 921, 932, 942, 943,
	948, 955, 963, 965, 967, 970, 975, 976, 985,
}

func Execute() error {
	o := &Opts{}
	args, err := flags.Parse(o)
	if err != nil {
		return err
	}

	if len(args) != 1 {
		return fmt.Errorf("Neead a file to read %v", args)
	}
	basedir := filepath.Dir(args[0])
	prev := filepath.Base(args[0])
	file, err := os.Open(args[0])
	if err != nil {
		return err
	}
	defer file.Close()

	uncomp, err := gzip.NewReader(file)
	if err != nil {
		return err
	}
	defer uncomp.Close()
	header := hermes.Header{}

	ok, err := hermes.ReadDelimitedMessage(uncomp, &header)
	if ok == false {
		return fmt.Errorf("Could not read version of file")
	}
	if err != nil {
		return err
	}
	if len(header.Previous) != 0 {
		fmt.Fprintf(os.Stderr, "WARNING: this file seems to have previous data '%s'", header.Previous)
	}

	for {
		line := hermes.FileLine{}
		ok, err = hermes.ReadDelimitedMessage(uncomp, &line)
		if ok == false {
			return fmt.Errorf("Could not read file line")
		}
		if err != nil {
			if err == io.EOF {
				return nil
			}
			return err
		}

		if line.Readout != nil {

			fmt.Fprintf(os.Stdout, "%d.%02d,%d,15,%d",
				line.Readout.Time.GetSeconds(),
				line.Readout.Time.GetNanos()/1e7,
				line.Readout.FrameID,
				len(line.Readout.Tags))
			for _, a := range line.Readout.Tags {
				ID := int(a.ID)
				if o.ARTagConvert == true {
					if ID < len(ARTagID100) {
						ID = ARTagID100[ID]
					} else {
						log.Printf("Unexpected tag ID %d", a.ID)
					}
				}

				fmt.Fprintf(os.Stdout, ",%d,%d,%d,%.2f",
					ID,
					int(a.X),
					int(a.Y),
					a.Theta*180.0/math.Pi)
			}
			fmt.Fprintf(os.Stdout, "\n")
		}
		if line.Footer == nil {
			continue
		}

		if len(line.Footer.Next) == 0 {
			//endof stream
			return nil
		}
		uncomp.Close()
		file.Close()
		next := filepath.Join(basedir, line.Footer.Next)
		fmt.Fprintf(os.Stderr, "opening next file '%s'\n", next)
		file, err = os.Open(next)
		if err != nil {
			return err
		}
		uncomp, err = gzip.NewReader(file)
		if err != nil {
			return err
		}
		ok, err = hermes.ReadDelimitedMessage(uncomp, &header)
		if ok == false {
			return fmt.Errorf("Could not read header")
		}
		if err != nil {
			return err
		}
		if header.Previous != prev {
			return fmt.Errorf("previous file for '%s' is '%s' but '%s' expected", next, header.Previous, prev)
		}
		prev = line.Footer.Next
	}

	return nil
}

func main() {
	if err := Execute(); err != nil {
		log.Fatalf("Unhandled error: %s", err)
	}
}