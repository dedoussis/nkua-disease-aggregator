#!/bin/bash
# Usage ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile

set -e

MAX_AGE=99

diseases_file=$1
countries_file=$2
input_dir=$3
num_files_per_dir=$4
num_records_per_file=$5

IFS=$'\r\n' GLOBIGNORE='*' command eval 'diseases=($(cat ${diseases_file})) && countries=($(cat ${countries_file}))'

declare -a record_types=(ENTER EXIT) 
declare -a first_names=(John George Paul Mary Lia Tris Kate Jess Chris Nick Jenny Rachel Emily Andrew Simon Alex Marin Samuel Lee Joe)
declare -a last_names=(Johnson Greenwood Rooney Martial Bate Williams Wood Kane Jones Clarke Davies Park Cole Colins Hopkins Klopp Dee)

record_count=0

echo "⌛ Generating records in ${input_dir}..."

mkdir $input_dir

for country in "${countries[@]}"; do
    country_dir=$input_dir/$country
    mkdir $country_dir
    declare -a entered=()
    for (( i=0; i<$num_files_per_dir; i++ )); do
        # The line below assumes the date utility of BSD Unix
        # For Linux this should change to $(date +%F' -d '+${i} days')
        file_name="$input_dir/$country/$(date -v +${i}d '+%F')"
        touch $file_name

        for (( j=0; j<$num_records_per_file; j++ )); do
            record_id=$RANDOM
            record_type=${record_types[$(($record_id%2))]}
            entered_size=${#entered[@]}

            if [[ "$record_type" == "${record_types[0]}" ]] || [ "$entered_size" -eq 0 ]; then
                record_type=${record_types[0]}
                first_name=${first_names[$(($record_id % ${#first_names[@]}))]}
                last_name=${last_names[$(($record_id % ${#last_names[@]}))]}
                disease=${diseases[$(($record_id % ${#diseases[@]}))]}
                age=$(($record_id % $MAX_AGE))
                patient="${first_name} ${last_name} ${disease} ${age}"
                entered+=("${patient}")
            else
                entered_idx=$(($record_id%$entered_size))
                patient=${entered[$entered_idx]}
                declare -a entered_tmp=()
                for (( k=0; k<$entered_size; k++ )); do
                    if [ "$k" -ne "$entered_idx" ]; then
                        entered_tmp+=("${entered[k]}")
                    fi  
                done
                entered=("${entered_tmp[@]}")
            fi

            echo "${record_id} ${record_type} ${patient}" >> $file_name
            record_count=$((record_count+1))
        done
    done
done

echo "✅ Successfully generated ${record_count} records"
