#!/usr/bin/env nextflow
process do_space {
    container 'pydocker'

    input:
    path file_in
    path space_in
    
    output:
    path '*.da2'
    
    script:
    """
    python3 /app/do_space.py ${file_in} ${space_in} ${file_in.name.replaceAll(/\.[^\\.]+$/, '.da2')}
    """
}