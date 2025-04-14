#!/usr/bin/env nextflow
process sample {
    
    container 'basedocker'
    
    input:
    path file_in
    val n_value
    
    output:
    'sample.dat'
    
    script:
    """
    /app/process_file s $n_value sample.dat $file_in
    """
}