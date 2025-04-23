#!/usr/bin/env nextflow
process create_space {
    
    container 'pydocker'
    
    input:
    path file_in
    
    output:
    path 'space.pk'
    
    script:
    """
    python3 /app/create_space.py ${file_in} space.pk
    """
}