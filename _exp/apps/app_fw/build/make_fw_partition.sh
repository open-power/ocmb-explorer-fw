#*******************************************************************************
#  Copyright 2018 Microchip Technology Inc. and its subsidiaries.
#  Subject to your compliance with these terms, you may use Microchip
#  software and any derivatives exclusively with Microchip products. It is
#  your responsibility to comply with third party license terms applicable to
#  your use of third party software (including open source software) that may
#  accompany Microchip software.
#  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
#  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
#  IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
#  PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
#  SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR
#  EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED,
#  EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
#  FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
#  LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
#  EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO
#  MICROCHIP FOR THIS SOFTWARE.
# -----------------------------------------------------------------------------
# DESCRIPTION  :  Script for generating Flash Image
#
# NOTES        :  None.
#
#*******************************************************************************/
#!/bin/bash
set -e


# Local variables
outfile_a=""
outfile_b=""
outfile_f=""
infile=""
DIRECTORY=out
partition_offset=0x40000
default_partition_flag=0x0
partition_flag_size=4
# For now Image offset accounts for 2KB security header.
# Once security header is implemented, the offset should be
# changed to 0x40800 and 0x161800
image_a_offset=0x40800
image_b_offset=0x161800
bytes_to_fill=0


function print_help () {
    echo "Usage: $0  <args>  where args are: "
    echo "   -i <input_file>     (required)"
    echo "   -a <JTAG Image A>    (required)"
	echo "   -b <JTAG Image B>    (required)"
	echo "   -f <Complete image containg BOOT Flag, Image A and IMAGE B>    (required)"
    echo "   -h                  Print this help."
	echo "   Example usage: make_fw_partition.sh -i app.mem -a app_jtag_A.out -b app_jtag_B.out -f app_full_mage.out"
	echo "	 where:	app.mem 			: Input binary File"
	echo "			app_jtag_A.out 		: Output JTAG Image A File"
	echo "			app_jtag_B.out 		: Output JTAG Image B File"
	echo "			app_full_mage.out 	: Output Full Image File"
}

# Process command-line arguments
while [ $# -gt 0 ]; do
    case "$1" in
    # Input and output files
    -a) outfile_a=$2; shift; ;;
	-b) outfile_b=$2; shift; ;;
	-f) outfile_f=$2; shift; ;;
    -i) infile=$2; shift; ;;
    -h) print_help; exit 0; ;;
    *) echo "$0: error - unrecognized option $1" 1>&2; print_help ; exit 1; ;;
    esac
    shift
done

if [[ -z $infile ]]; then
    printf "\n**** ERROR:  No input file specified  ****\n"
    print_help
    exit 1
fi
if [[ -z $outfile_a ]]; then
    printf "\n**** ERROR:  No JTAG Image A file specified  ****\n"
    print_help
    exit 1
fi

if [[ -z $outfile_b ]]; then
    printf "\n**** ERROR:  No JTAG Image B file specified  ****\n"
    print_help
    exit 1
fi

if [[ -z $outfile_f ]]; then
    printf "\n**** ERROR:  No Full Image file specified  ****\n"
    print_help
    exit 1
fi

if [ ! -d "$DIRECTORY" ]; then
  echo Creating output directory
  mkdir out
else
	echo Cleaning output directory
	rm -rf .//$DIRECTORY/*
fi

# Helper function to print a specified number of raw bytes
function print_bytes() {
    val="\x"$(printf "%02x" $1)
    for (( x=0; x<$2; x++ )); do
        echo -en $val;
    done
}

# Flash Layout for Explorer
# Offset(KB)  size             descriptin
# =========  ======           ============
# 0           256KB            : Flash PBOOT Image
# 256   	    2KB            : ACTIV Image flag.
# 258           2KB            : FW Image A Header
# 260        1024KB            : FW Image A
# 1284        128KB            : Config DATA & FW LOG A
# 1412          2KB            : Reserved
# 1414          2KB            : FW Image B Header
# 1416   	 1024KB            : FW Image B
# 2440        128KB            : Config DATA & FW LOG B
# 2568       1024KB            : Temp location
# 3592          0KB            : End of Flash



#create JTAG Image A
printf "\n**** Generating JTAG Image A file:  "
cp	$infile .//$DIRECTORY//$outfile_a
printf "Success!!! ****\n"

#create JTAG Image B
printf "\n**** Generating JTAG Image B file:  "
cp	$infile .//$DIRECTORY//$outfile_b
printf "Success!!! ****\n"

#create full Image
printf "\n**** Generating Full Flash Image file:  "

# Calculate size of input file
insize=$(wc -c < $infile)
#insize=$(printf "%08x" $insize)

# 256KB PBOOT Space
print_bytes 0xFF $partition_offset >> .//$DIRECTORY//$outfile_f
# Add default Partition flag
print_bytes $default_partition_flag $partition_flag_size >> .//$DIRECTORY//$outfile_f

# Fill the space between Partion flag and Image A with 0xFF
bytes_to_fill=$(((image_a_offset - partition_offset) -partition_flag_size))
print_bytes 0xFF $bytes_to_fill >> .//$DIRECTORY//$outfile_f
# Add Image A file
cat .//$DIRECTORY//$outfile_a >> .//$DIRECTORY//$outfile_f
# Fill the space between Image A and Image B with 0xFF
bytes_to_fill=$(((image_b_offset - image_a_offset) -insize))
print_bytes 0xFF $bytes_to_fill >> .//$DIRECTORY//$outfile_f
# Add Image B file
cat .//$DIRECTORY//$outfile_b >> .//$DIRECTORY//$outfile_f

printf "Success!!! ****\n"

