#!/bin/bash

set -e

HELP_DESCRIPTION="Usage ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile"

diseases_file=$1
countries_file=$2
input_dir=$3
num_files_per_dir=$4
num_records_per_file=$5

IFS=$'\r\n' GLOBIGNORE='*' command eval 'diseases=($(cat ${diseases_file})) && countries=($(cat ${countries_file}))'

declare -a record_types=(ENTER EXIT) 

diseases_size=${#diseases[@]}
countries_size=${#countries[@]}

echo "⌛ Generating records in ${input_dir}..."

for country in "${countries[@]}"; do
    country_dir=$input_dir/$country
    mkdir $country_dir
    for (( i=0; i<$num_files_per_dir; i++ )); do
        file_name=$input_dir/$country/$i
        touch $file_name
        for (( j=0; j<$num_files_per_dir; j++ )); do
            record_id=$RANDOM
            record_type=${record_types[$(($record_id%2))]}
            first_name=`LC_ALL=C tr -dc 'a-z' </dev/urandom | head -c 5; echo`
            last_name=`LC_ALL=C tr -dc 'a-z' </dev/urandom | head -c 5; echo`
            disease=${diseases[$(($record_id%$diseases_size))]}
            age=$((($record_id%99)+1))
            echo "${record_id} ${record_type} ${first_name} ${last_name} ${disease} ${age}" >> $file_name
        done
    done
done

records_size=$(($countries_size*$num_files_per_dir*$num_records_per_file))
echo "✅ Successfully generated ${records_size} records"
