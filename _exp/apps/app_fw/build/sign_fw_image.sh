#*******************************************************************************
#  Copyright 2019 Microchip Technology Inc. and its subsidiaries.
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
# DESCRIPTION  :  Script for signing firmware image
#
# NOTES        :  None.
#
#*******************************************************************************/
#!/bin/bash
set -e

# Local variables
gobinz_app=""
outfile_b=""
outfile_f=""
infile=""
csv=""

function print_help () {
    echo "Usage: $0  <args>  where args are: "
    echo "   -a <gobinz.exe application (required)"
    echo "   -i <input_file>     (required)"
    echo "   -o <output_file>    (required)"
    echo "   -c <csv>            (required)"
    echo "   -h                  Print this help."
    echo "   Example usage: sign_fw_image.sh -a gobinz.exe -i app_fw.mem -o signed_app_fw.mem -c app_fw_key1.csv"
    echo "   where: gobinz.exe          : Path to gobinz Application"
    echo "          app_fw.mem          : Input binary File"
    echo "          signed_app_fw.mem   : Signed output binary File"
    echo "          app_fw_key1.csv     : CSV containing signing data"
}

# Process command-line arguments
while [ $# -gt 0 ]; do
    case "$1" in
    # Input and output files
    -a) gobinz_app=$2; shift; ;;
    -o) outfile=$2; shift; ;;
    -i) infile=$2; shift; ;;
    -c) csv=$2; shift; ;;
    -h) print_help; exit 0; ;;
    *) echo "$0: error - unrecognized option $1" 1>&2; print_help ; exit 1; ;;
    esac
    shift
done

if [[ -z $gobinz_app ]]; then
    printf "\n**** ERROR:  No GOBINZ app specified  ****\n"
    print_help
    exit 1
fi

if [[ -z $infile ]]; then
    printf "\n**** ERROR:  No input file specified  ****\n"
    print_help
    exit 1
fi

if [[ -z $outfile ]]; then
    printf "\n**** ERROR:  No output file specified  ****\n"
    print_help
    exit 1
fi

if [[ -z $csv ]]; then
    printf "\n**** ERROR:  No CSV file specified  ****\n"
    print_help
    exit 1
fi

#clean up previous signed image
if [ -f "$outfile" ]; then
    rm $outfile
fi

#Get token 
printf "\n**** Generating token ****\n"
$gobinz_app login-vault --role securesign-explorer-dev

#Sign image
printf "\n**** Signing $infile ****\n"
$gobinz_app generate -i $infile -o $outfile $csv --noreverse

if [ -f "$outfile" ]; then
    printf "$outfile has been successfully created\n"
else
    printf "Failed to sign image $infile!\n"
fi
