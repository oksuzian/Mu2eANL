#!/bin/bash

for suffix in PE SPE Aging Temp; do
    for config in 001ext 006 007; do
 
        output_file="${suffix}-${config}"
        output_htmlfile="${suffix}-${config}.html"
        output_dir="/mu2e/app/users/oksuzian/Mu2eANL/Wideband"
        web_dir="/web/sites/mu2e.fnal.gov/htdocs/atwork/workgroups/Wideband"
        dataset="rec.mu2e.CRV_wideband_cosmics.CRVWB-000-001-000-${config}.root"

        papermill ${output_dir}/${suffix}.ipynb ${output_dir}/${output_file}.ipynb -p DATASET "${dataset}"
        jupyter nbconvert --to html ${output_dir}/${output_file}.ipynb --output-dir ${web_dir} --no-input
        date=$(date +%Y-%m-%d)
        cp ${web_dir}/${output_file}.html  ${web_dir}/${output_file}_${date}.html

    done
done
