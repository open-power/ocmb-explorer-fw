import sys
import os.path as opath
import codecs
import struct

NUM_BYTE_ECC = 4
NOT_USED_FILL = 0x00

def data_mask(d, p):
    assert type(p) is int
    return ((d & (1 << p)) >> p)

"""
P0 = D0 + D1 + D3 + D4 + D6 + D8 + D10 + D11 + D13 + D15 + D17 + D19 + D21 + D23 + D25 + D26 + D28 + D30
P1 = D0 + D2 + D3 + D5 + D6 + D9 + D10 + D12 + D13 + D16 + D17 + D20 + D21 + D24 + D25 + D27 + D28 + D31
P2 = D1 + D2 + D3 + D7 + D8 + D9 + D10 + D14 + D15 + D16 + D17 + D22 + D23 + D24 + D25 + D29 + D30 + D31
P3 = D4 + D5 + D6 + D7 + D8 + D9 + D10 + D18 + D19 + D20 + D21 + D22 + D23 + D24 + D25
P4 = D11 + D12 + D13 + D14 + D15 + D16 + D17 + D18 + D19 + D20 + D21 + D22 + D23 + D24 + D25
P5 = D26 + D27 + D28 + D29 + D30 + D31
P6 = D0 + D1 + D2 + D3 + D4 + D5 + D6 + D7 + D8 + D9 + D10 + D11 + D12 + 
     D13 + D14 + D15 + D16 + D17 + D18 + D19 + D20 + D21 + D22 + D23 + 
     D24 + D25 + D26 + D27 + D28 + D29 + D30 + D31 + P0 + P1 + P2 + P3 + P4 + P5
"""
def hamming_code(data_input):
    assert type(data_input) is int or type(data_input) is long

    P0 = (data_mask(data_input, 0)) ^ (data_mask(data_input, 1)) ^ (data_mask(data_input, 3)) ^ (data_mask(data_input, 4)) ^ (data_mask(data_input, 6)) ^ \
		(data_mask(data_input, 8)) ^ (data_mask(data_input, 10)) ^ (data_mask(data_input, 11)) ^ (data_mask(data_input, 13)) ^ (data_mask(data_input, 15)) ^ \
		(data_mask(data_input, 17)) ^ (data_mask(data_input, 19)) ^ (data_mask(data_input, 21)) ^ (data_mask(data_input, 23)) ^ (data_mask(data_input, 25)) ^ \
		(data_mask(data_input, 26)) ^ (data_mask(data_input, 28)) ^ (data_mask(data_input, 30));

    P1 = (data_mask(data_input, 0)) ^ (data_mask(data_input, 2)) ^ (data_mask(data_input, 3)) ^ (data_mask(data_input, 5)) ^ (data_mask(data_input, 6)) ^ \
		(data_mask(data_input, 9)) ^ (data_mask(data_input, 10)) ^ (data_mask(data_input, 12)) ^ (data_mask(data_input, 13)) ^ (data_mask(data_input, 16)) ^ \
		(data_mask(data_input, 17)) ^ (data_mask(data_input, 20)) ^ (data_mask(data_input, 21)) ^ (data_mask(data_input, 24)) ^ (data_mask(data_input, 25)) ^ \
		(data_mask(data_input, 27)) ^ (data_mask(data_input, 28)) ^ (data_mask(data_input, 31));

    P2 = (data_mask(data_input, 1)) ^ (data_mask(data_input, 2)) ^ (data_mask(data_input, 3)) ^ (data_mask(data_input, 7)) ^ (data_mask(data_input, 8)) ^ \
		(data_mask(data_input, 9)) ^ (data_mask(data_input, 10)) ^ (data_mask(data_input, 14)) ^ (data_mask(data_input, 15)) ^ (data_mask(data_input, 16)) ^ \
		(data_mask(data_input, 17)) ^ (data_mask(data_input, 22)) ^ (data_mask(data_input, 23)) ^ (data_mask(data_input, 24)) ^ (data_mask(data_input, 25)) ^ \
		(data_mask(data_input, 29)) ^ (data_mask(data_input, 30)) ^ (data_mask(data_input, 31));

    P3 = (data_mask(data_input, 4)) ^ (data_mask(data_input, 5)) ^ (data_mask(data_input, 6)) ^ (data_mask(data_input, 7)) ^ (data_mask(data_input, 8)) ^ \
		(data_mask(data_input, 9)) ^ (data_mask(data_input, 10)) ^ (data_mask(data_input, 18)) ^ (data_mask(data_input, 19)) ^ (data_mask(data_input, 20)) ^ \
		(data_mask(data_input, 21)) ^ (data_mask(data_input, 22)) ^ (data_mask(data_input, 23)) ^ (data_mask(data_input, 24)) ^ (data_mask(data_input, 25));

    P4 = (data_mask(data_input, 11)) ^ (data_mask(data_input, 12)) ^ (data_mask(data_input, 13)) ^ (data_mask(data_input, 14)) ^ (data_mask(data_input, 15)) ^ \
		(data_mask(data_input, 16)) ^ (data_mask(data_input, 17)) ^ (data_mask(data_input, 18)) ^ (data_mask(data_input, 19)) ^ (data_mask(data_input, 20)) ^ \
		(data_mask(data_input, 21)) ^ (data_mask(data_input, 22)) ^ (data_mask(data_input, 23)) ^ (data_mask(data_input, 24)) ^ (data_mask(data_input, 25));

    P5 = (data_mask(data_input, 26)) ^ (data_mask(data_input, 27)) ^ (data_mask(data_input, 28)) ^ (data_mask(data_input, 29)) ^ (data_mask(data_input, 30)) ^ \
		(data_mask(data_input, 31));

    P6 = (data_mask(data_input, 0)) ^ (data_mask(data_input, 1)) ^ (data_mask(data_input, 2)) ^ (data_mask(data_input, 3)) ^ (data_mask(data_input, 4)) ^ \
		(data_mask(data_input, 5)) ^ (data_mask(data_input, 6)) ^ (data_mask(data_input, 7)) ^ (data_mask(data_input, 8)) ^ (data_mask(data_input, 9)) ^ \
		(data_mask(data_input, 10)) ^ (data_mask(data_input, 11)) ^ (data_mask(data_input, 12)) ^ (data_mask(data_input, 13)) ^ (data_mask(data_input, 14)) ^ \
		(data_mask(data_input, 15)) ^ (data_mask(data_input, 16)) ^ (data_mask(data_input, 17)) ^ (data_mask(data_input, 18)) ^ (data_mask(data_input, 19)) ^ \
		(data_mask(data_input, 20)) ^ (data_mask(data_input, 21)) ^ (data_mask(data_input, 22)) ^ (data_mask(data_input, 23)) ^ (data_mask(data_input, 24)) ^ \
		(data_mask(data_input, 25)) ^ (data_mask(data_input, 26)) ^ (data_mask(data_input, 27)) ^ (data_mask(data_input, 28)) ^ (data_mask(data_input, 29)) ^ \
		(data_mask(data_input, 30)) ^ (data_mask(data_input, 31)) ^ (data_mask(P0, 0)) ^ (data_mask(P1, 0)) ^ (data_mask(P2, 0)) ^ (data_mask(P3, 0)) ^ \
		(data_mask(P4, 0)) ^ (data_mask(P5, 0));
    
    ham_value = (data_mask(P0, 0)) | ((data_mask(P1, 0)) << 1) | ((data_mask(P2, 0)) << 2) | ((data_mask(P3, 0)) << 3) | \
		((data_mask(P4, 0)) << 4) | ((data_mask(P5, 0)) << 5) | ((data_mask(P6, 0)) << 6);
    
    assert ham_value >= 0x0 and ham_value <= 0xff
    return ham_value

if type(sys.argv[1]) is not str:
    sys.argv[1] = str(sys.argv[1])

try:
    val = int(sys.argv[1], 16)
    print("val " + hex(val) + " hamming_code " + hex(hamming_code(val)))
    sys.exit(0)
except ValueError:
    pass

pathToSignedImage = opath.expanduser(sys.argv[1].replace("\\", "/"))
# Need to handle case where start address is in the middle of a 32-byte block
# startAddr is the PHYSICAL start address of the ECC image, not LOGICAL
try:
    startAddr = int(sys.argv[2])
except ValueError:
    startAddr = int(sys.argv[2], 16)

assert opath.isfile(pathToSignedImage), \
    "\n\nUsage: python eccCov.py [signed_image_name or /path/to/signed_image]\n"

input_filename = opath.basename(pathToSignedImage);
input_filename_truncated = input_filename[:-4]
input_filename_extension = input_filename[-4:]
pathToSignedEccImage = opath.join(opath.dirname(pathToSignedImage), \
                                  (input_filename_truncated + "_ecc" + input_filename_extension))

with open(pathToSignedImage, "rb") as fd:
    byteArray = bytearray(fd.read())

with open(pathToSignedEccImage, "wb") as fd:
    # In case the physical start address is in the middle of a 32-byte bolck,
    # we have to take this offset into account so that the NOT_USED_FILL at
    # location 30 and 31 are not filled by meaningful bytes. 
    count = startAddr % 32
    for i in range(0, len(byteArray), NUM_BYTE_ECC):
        eccByte = 0
        for j in range(NUM_BYTE_ECC):
            fd.write(struct.pack("B", (byteArray[i + j])))
            #print(byteArray[i + j])
            # Shift left by 8 bits for each byte, up to NUM_BYTE_ECC bytes
            eccByte |= byteArray[i + j] << (j * 8)

        fd.write(struct.pack("B", hamming_code(eccByte)))
        count += NUM_BYTE_ECC + 1
        if count == 30:
            fd.write(struct.pack("B", NOT_USED_FILL))
            fd.write(struct.pack("B", NOT_USED_FILL))
            count = 0

#print("Make sure that the original image has been signed before the conversion!")
#print(pathToSignedEccImage)
sys.exit(0)
