#!/usr/bin/env nextflow
nextflow.enable.dsl=2
params.pre = params.pre ?: false
include { pre1 } from './process/pre1.nf'
include { binfile } from './process/binfile.nf'
workflow {
    
    if (params.pre) {
        csv_files = Channel.fromPath('data/*.csv')
        pre1(csv_files)
        binfile(csv_files)
    }else{
        dat_dir = Channel.value(file('data'))
    }
    
}
