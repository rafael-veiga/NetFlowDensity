#!/usr/bin/env nextflow
nextflow.enable.dsl=2

// Importa o módulo 'pre1' do arquivo process/pre1.nf
include { pre1 } from './process/pre1.nf'

workflow {
    // Cria um canal com todos os arquivos .csv na pasta 'data'
    csv_files = Channel.fromPath('data/*.csv')
    
    // Chama o módulo pre1 passando o canal criado
    pre1(csv_files)
}
