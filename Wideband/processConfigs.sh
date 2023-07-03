#!/bin/bash

input_dir="/home/oksuzian/Mu2eANL/Wideband"
output_dir="/home/oksuzian/Mu2eANL/Wideband/aging_nb"
web_dir="/web/sites/mu2e.fnal.gov/htdocs/atwork/workgroups/Wideband"
kernel="MyEnv"

for suffix in PE SPE Aging Temp; do
    for config in 001ext 006 007; do
 
        output_file="${suffix}-${config}"
        output_htmlfile="${suffix}-${config}.html"

        dataset="rec.mu2e.CRV_wideband_cosmics.CRVWB-000-003-000-${config}.root"
        papermill ${input_dir}/${suffix}.ipynb ${output_dir}/${output_file}.ipynb -p DATASET "${dataset}" -k $kernel
        jupyter nbconvert --to html ${output_dir}/${output_file}.ipynb --output-dir ${output_dir} --no-input
        date=$(date +%Y-%m-%d)
        scp ${output_dir}/${output_file}.html mu2egpvm03.fnal.gov:/${web_dir}/${output_file}.html
        scp ${output_dir}/${output_file}.html mu2egpvm03.fnal.gov:/${web_dir}/${output_file}_${date}.html

    done
done
