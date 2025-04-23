#!/usr/bin/env nextflow
nextflow.enable.dsl=2
params.pre = false
params.nsample = 10000
include { pre1 } from './process/pre1.nf'
include { binfile } from './process/binfile.nf'
include { sample } from './process/sample.nf'
include { create_space } from './process/create_space.nf'

workflow {
    if (params.pre) {
        csv_files = Channel.fromPath('data/*.csv')
        pre1_files = pre1(csv_files)
        bin_files_ch = binfile(pre1_files)
        bin_files_list = bin_files_ch.collect()
        sample_ch = sample(bin_files_list, params.nsample)
        space_ch = create_space(sample_ch)
    }else{
        dat_dir = Channel.value(file('data'))
    }
    
}
