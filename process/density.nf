#!/usr/bin/env nextflow
process density {
    container 'linuxcontainers/debian-slim:12.5'
    publishDir "data_aux", mode: 'copy'

    input:
    path file_in
    
    output:
    path '*.dad'
    
    script:
    """
    /app/density 500 ${file_in} ${file_in.name.replaceAll(/\.[^\\.]+$/, '.dad')} -20 20 -20 20
    """
}