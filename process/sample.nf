#!/usr/bin/env nextflow
process sample {
    
    container 'basedocker'
    
    publishDir "${workflow.projectDir}", mode: 'copy'
    
    input:
    path file_in
    val n_value
    
    output:
    path 'sample.dat'
    
    script:
    """
    /app/process_file s $file_in $n_value sample.dat
    """
}