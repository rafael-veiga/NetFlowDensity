#!/usr/bin/env nextflow
process sample {
    
    container 'linuxcontainers/debian-slim:12.5'
    
    input:
    path file_in
    val n_value
    
    output:
    path 'sample.dat'
    
    script:
    """
    /app/process_file s $n_value sample.dat $file_in
    """
}