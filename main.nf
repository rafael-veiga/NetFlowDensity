#!/usr/bin/env nextflow
nextflow.enable.dsl=2
params.pre = false
params.nsample = 10000
include { pre1 } from './process/pre1.nf'
include { binfile } from './process/binfile.nf'
include { sample } from './process/sample.nf'

workflow {
    
    if (params.pre) {
        csv_files = Channel.fromPath('data/*.csv')
        pre1_files = pre1(csv_files)
        bin_files_ch = binfile(pre1_files)
        bin_files_list = bin_files_ch.collect()
        bin_files_list.view()
        sample(bin_files_list, params.nsample)
        //bin_files_list.view()
    }else{
        dat_dir = Channel.value(file('data'))
    }
    
}
