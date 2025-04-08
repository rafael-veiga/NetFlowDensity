#!/usr/bin/env nextflow
process binfile {
    
    container 'basedocker'
    
    publishDir "${workflow.projectDir}/data_aux", mode: 'copy'
    
    input:
    path file_in
    
    output:
    path '*.dat'
    
    script:
    """
    file_name=\$(basename ${file_in} .csv).dat
    /app/process_file r ${file_in} \$file_name
    """
}