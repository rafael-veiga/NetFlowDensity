#!/usr/bin/env nextflow
nextflow.enable.dsl=2
params.pre = params.pre ?: false
params.nsample = params.nsample ?: 10000
include { pre1 } from './process/pre1.nf'
include { binfile } from './process/binfile.nf'
include { sample } from './process/sample.nf'

workflow {
    
    if (params.pre) {
        csv_files = Channel.fromPath('data/*.csv')
        pre1(csv_files)
        csv_files.collect()
        bin_files_ch = binfile(csv_files)
        bin_files_ch
            .collect()
            .map { files ->
                // Assumindo que todos foram para a mesma pasta
                def out_dir = files[0].getParent()
                return out_dir
            }
            .set { bin_out_dir_ch }
        sample(bin_out_dir_ch, params.nsample)

        
    }else{
        dat_dir = Channel.value(file('data'))
    }
    
}
