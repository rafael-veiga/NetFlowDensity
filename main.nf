#!/usr/bin/env nextflow
nextflow.enable.dsl=2
params.pre = params.pre ?: false
include { pre1 } from './process/pre1.nf'
include { binfile } from './process/binfile.nf'
workflow {
    csv_files = Channel.fromPath('data/*.csv')
    if (params.pre) {
        pre1(csv_files)
    }
    binfile(csv_files)
}
