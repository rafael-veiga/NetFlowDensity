#!/usr/bin/env nextflow
process binfile {
    
    container 'linuxcontainers/debian-slim:12.5'

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