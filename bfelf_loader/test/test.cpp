//
// Bareflank Hypervisor
//
// Copyright (C) 2015 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
// Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <test.h>
#include <fstream>
#include <sys/mman.h>

const auto c_dummy_misc_filename = "../cross/libdummy_misc.so";
const auto c_dummy_code_filename = "../cross/libdummy_code.so";

bfelf_loader_ut::bfelf_loader_ut() :
    m_dummy_misc_length(0),
    m_dummy_code_length(0)
{
}

bool bfelf_loader_ut::init()
{
    std::ifstream dummy_misc_ifs(c_dummy_misc_filename, std::ifstream::ate);
    std::ifstream dummy_code_ifs(c_dummy_code_filename, std::ifstream::ate);

    m_dummy_misc_length = static_cast<uint64_t>(dummy_misc_ifs.tellg());
    m_dummy_code_length = static_cast<uint64_t>(dummy_code_ifs.tellg());

    m_dummy_misc = std::make_unique<char[]>(m_dummy_misc_length);
    m_dummy_code = std::make_unique<char[]>(m_dummy_code_length);

    dummy_misc_ifs.seekg(0);
    dummy_code_ifs.seekg(0);

    dummy_misc_ifs.read(m_dummy_misc.get(), static_cast<int64_t>(m_dummy_misc_length));
    dummy_code_ifs.read(m_dummy_code.get(), static_cast<int64_t>(m_dummy_code_length));

    return true;
}

bool bfelf_loader_ut::fini()
{
    return true;
}

bool bfelf_loader_ut::list()
{
    this->test_bfelf_file_init_success();
    this->test_bfelf_file_init_invalid_file_arg();
    this->test_bfelf_file_init_invalid_file_size_arg();
    this->test_bfelf_file_init_invalid_elf_file();
    this->test_bfelf_file_init_invalid_magic_0();
    this->test_bfelf_file_init_invalid_magic_1();
    this->test_bfelf_file_init_invalid_magic_2();
    this->test_bfelf_file_init_invalid_magic_3();
    this->test_bfelf_file_init_invalid_class();
    this->test_bfelf_file_init_invalid_data();
    this->test_bfelf_file_init_invalid_ident_version();
    this->test_bfelf_file_init_invalid_osabi();
    this->test_bfelf_file_init_invalid_abiversion();
    this->test_bfelf_file_init_invalid_type();
    this->test_bfelf_file_init_invalid_machine();
    this->test_bfelf_file_init_invalid_version();
    this->test_bfelf_file_init_invalid_flags();

    this->test_bfelf_file_num_segments_invalid_ef();
    this->test_bfelf_file_num_segments_uninitalized();
    this->test_bfelf_file_num_segments_success();

    this->test_bfelf_file_get_segment_invalid_ef();
    this->test_bfelf_file_get_segment_invalid_index();
    this->test_bfelf_file_get_segment_invalid_phdr();
    this->test_bfelf_file_get_segment_success();

    this->test_bfelf_file_resolve_symbol_invalid_loader();
    this->test_bfelf_file_resolve_symbol_invalid_name();
    this->test_bfelf_file_resolve_symbol_invalid_addr();
    this->test_bfelf_file_resolve_no_such_symbol_no_relocation();
    this->test_bfelf_file_resolve_no_such_symbol();
    this->test_bfelf_file_resolve_symbol_success();
    this->test_bfelf_file_resolve_no_such_symbol_no_hash();
    this->test_bfelf_file_resolve_symbol_success_no_hash();

    this->test_bfelf_loader_add_invalid_loader();
    this->test_bfelf_loader_add_invalid_elf_file();
    this->test_bfelf_loader_add_too_many_files();

    this->test_bfelf_loader_relocate_invalid_loader();
    this->test_bfelf_loader_relocate_no_files_added();
    this->test_bfelf_loader_relocate_uninitialized_files();
    this->test_bfelf_loader_relocate_twice();

    this->test_bfelf_file_get_section_info_invalid_elf_file();
    this->test_bfelf_file_get_section_info_invalid_info();
    this->test_bfelf_file_get_section_info_expected_misc_resources();
    this->test_bfelf_file_get_section_info_expected_code_resources();

    this->test_bfelf_loader_resolve_symbol_invalid_loader();
    this->test_bfelf_loader_resolve_symbol_invalid_name();
    this->test_bfelf_loader_resolve_symbol_invalid_addr();
    this->test_bfelf_loader_resolve_symbol_no_files_added();
    this->test_bfelf_loader_resolve_symbol_uninitialized_files();
    this->test_bfelf_loader_resolve_no_such_symbol();
    this->test_bfelf_loader_resolve_symbol_success();
    this->test_bfelf_loader_resolve_no_such_symbol_no_hash();
    this->test_bfelf_loader_resolve_symbol_success_no_hash();
    this->test_bfelf_loader_resolve_symbol_real_test();

    // this->test_private_bfelf_error();
    // this->test_private_invalid_symbol_index();
    // this->test_private_corrupt_symbol_table();
    // this->test_private_relocate_invalid_index();
    // this->test_private_relocate_invalid_name();
    // this->test_private_relocate_invalid_relocation();
    // this->test_private_get_section_invalid_name();
    // this->test_private_symbol_table_sections_invalid_dynsym();
    // this->test_private_symbol_table_sections_invalid_hash();
    // this->test_private_string_table_sections_invalid();
    // this->test_private_get_relocation_tables_invalid_type();
    // this->test_private_get_relocation_tables_invalid_section();
    // this->test_private_hash();

    return true;
}

char *
alloc_exec(size_t size)
{
    auto addr = malloc(size);

    auto &&addr_adjusted = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(addr) & 0xFFFFFFFFFFFFF000);
    auto &&size_adjusted = size + (reinterpret_cast<uintptr_t>(addr) & 0xFFF);

    memset(addr, 0, size);
    mprotect(addr_adjusted, size_adjusted, PROT_READ | PROT_WRITE | PROT_EXEC);

    return reinterpret_cast<char *>(addr);
}

std::pair<std::unique_ptr<char[]>, uint64_t>
bfelf_loader_ut::get_elf_exec(bfelf_file_t *ef)
{
    auto &&total = 0UL;
    auto &&num_segments = bfelf_file_num_segments(ef);

    for (auto i = 0; i < num_segments; i++)
    {
        auto &&ret = 0L;
        bfelf_phdr *phdr = nullptr;

        ret = bfelf_file_get_segment(ef, i, &phdr);
        if (ret != BFELF_SUCCESS)
            throw std::runtime_error("bfelf_file_get_segment failed");

        if (total < phdr->p_vaddr + phdr->p_memsz)
            total = phdr->p_vaddr + phdr->p_memsz;
    }

    auto &&exec = std::unique_ptr<char[]>(alloc_exec(total));

    for (auto i = 0UL; i < total; i++)
        exec[i] = 0;

    for (auto i = 0U; i < num_segments; i++)
    {
        auto &&ret = 0L;
        bfelf_phdr *phdr = nullptr;

        ret = bfelf_file_get_segment(ef, i, &phdr);
        if (ret != BFELF_SUCCESS)
            throw std::runtime_error("bfelf_file_get_segment failed");

        auto &&exec_view = gsl::make_span(exec, gsl::narrow_cast<std::ptrdiff_t>(total));
        auto &&file_view = gsl::make_span(ef->file, gsl::narrow_cast<std::ptrdiff_t>(ef->filesz));

        memcpy(&exec_view.at(phdr->p_vaddr), &file_view.at(phdr->p_offset), phdr->p_filesz);
    }

    return {std::move(exec), total};
}

#define offset(a,b) ( (reinterpret_cast<uintptr_t>(&(a))) - (reinterpret_cast<uintptr_t>(&(b))) )

std::pair<std::unique_ptr<char[]>, uint64_t>
bfelf_loader_ut::get_test()
{
    auto &&size = sizeof(bfelf_test);
    auto &&buff = std::make_unique<char[]>(size);
    auto &&test = reinterpret_cast<bfelf_test *>(buff.get());

    test->header.e_ident[bfei_mag0] = 0x7F;
    test->header.e_ident[bfei_mag1] = 'E';
    test->header.e_ident[bfei_mag2] = 'L';
    test->header.e_ident[bfei_mag3] = 'F';
    test->header.e_ident[bfei_class] = bfelfclass64;
    test->header.e_ident[bfei_data] = bfelfdata2lsb;
    test->header.e_ident[bfei_version] = bfev_current;
    test->header.e_ident[bfei_osabi] = bfelfosabi_sysv;
    test->header.e_ident[bfei_abiversion] = 0;
    test->header.e_type = bfet_dyn;
    test->header.e_machine = bfem_x86_64;
    test->header.e_version = bfev_current;
    test->header.e_entry = 0x150;
    test->header.e_phoff = offset(test->phdrtab, *test);
    test->header.e_shoff = offset(test->shdrtab, *test);
    test->header.e_flags = 0;
    test->header.e_ehsize = sizeof(bfelf_ehdr);
    test->header.e_phentsize = sizeof(bfelf_phdr);
    test->header.e_phnum = sizeof(test_phdrtab) / sizeof(bfelf_phdr);
    test->header.e_shentsize = sizeof(bfelf_shdr);
    test->header.e_shnum = sizeof(test_shdrtab) / sizeof(bfelf_shdr);
    test->header.e_shstrndx = 3;

    test->phdrtab.re_segment.p_type = bfpt_load;
    test->phdrtab.re_segment.p_flags = bfpf_r | bfpf_x;
    test->phdrtab.re_segment.p_offset = 0;
    test->phdrtab.re_segment.p_vaddr = 0;
    test->phdrtab.re_segment.p_paddr = 0;
    test->phdrtab.re_segment.p_filesz = 0x500;
    test->phdrtab.re_segment.p_memsz = 0x500;
    test->phdrtab.re_segment.p_align = 0x1000;

    test->phdrtab.rw_segment.p_type = bfpt_load;
    test->phdrtab.rw_segment.p_flags = bfpf_r | bfpf_w;
    test->phdrtab.rw_segment.p_offset = 0x500;
    test->phdrtab.rw_segment.p_vaddr = 0x500;
    test->phdrtab.rw_segment.p_paddr = 0x500;
    test->phdrtab.rw_segment.p_filesz = 0x500;
    test->phdrtab.rw_segment.p_memsz = 0x500;
    test->phdrtab.rw_segment.p_align = 0x1000;

    test->phdrtab.dyn_segment.p_type = bfpt_dynamic;
    test->phdrtab.dyn_segment.p_flags = bfpf_r | bfpf_w;
    test->phdrtab.dyn_segment.p_offset = offset(test->eh_frame, *test);
    test->phdrtab.dyn_segment.p_vaddr = 0x0;
    test->phdrtab.dyn_segment.p_paddr = 0x0;
    test->phdrtab.dyn_segment.p_filesz = sizeof(test_dynamic);
    test->phdrtab.dyn_segment.p_memsz = sizeof(test_dynamic);
    test->phdrtab.dyn_segment.p_align = 0x1000;

    test->phdrtab.stack_segment.p_type = bfpt_gnu_stack;
    test->phdrtab.stack_segment.p_flags = bfpf_r | bfpf_w;
    test->phdrtab.stack_segment.p_offset = 0x0;
    test->phdrtab.stack_segment.p_vaddr = 0x0;
    test->phdrtab.stack_segment.p_paddr = 0x0;
    test->phdrtab.stack_segment.p_filesz = 0x0;
    test->phdrtab.stack_segment.p_memsz = 0x0;
    test->phdrtab.stack_segment.p_align = 0x0;

    test->phdrtab.relro_segment.p_type = bfpt_gnu_relro;
    test->phdrtab.relro_segment.p_flags = bfpf_r;
    test->phdrtab.relro_segment.p_offset = 0x0;
    test->phdrtab.relro_segment.p_vaddr = 0x0;
    test->phdrtab.relro_segment.p_paddr = 0x0;
    test->phdrtab.relro_segment.p_filesz = 0x500;
    test->phdrtab.relro_segment.p_memsz = 0x500;
    test->phdrtab.relro_segment.p_align = 0x1000;

    test->dynamic.needed1.d_tag = bfdt_needed;
    test->dynamic.needed2.d_tag = bfdt_needed;
    test->dynamic.pltgot.d_tag = bfdt_pltgot;
    test->dynamic.hash.d_tag = bfdt_hash;
    test->dynamic.hash.d_ptr = offset(test->hash, *test);
    test->dynamic.strtab.d_tag = bfdt_strtab;
    test->dynamic.strtab.d_ptr = offset(test->dynstr, *test);
    test->dynamic.symtab.d_tag = bfdt_symtab;
    test->dynamic.symtab.d_ptr = offset(test->dynsym, *test);
    test->dynamic.rela.d_tag = bfdt_rela;
    test->dynamic.rela.d_ptr = offset(test->relatab, *test);
    test->dynamic.relasz.d_tag = bfdt_relasz;
    test->dynamic.relasz.d_val = sizeof(test_relatab);
    test->dynamic.relaent.d_tag = bfdt_relaent;
    test->dynamic.relaent.d_val = sizeof(bfelf_rela);
    test->dynamic.strsz.d_tag = bfdt_strsz;
    test->dynamic.strsz.d_val = sizeof(test_dynstr);
    test->dynamic.syment.d_tag = bfdt_syment;
    test->dynamic.syment.d_val = sizeof(bfelf_sym);
    test->dynamic.bind_now.d_tag = bfdt_bind_now;
    test->dynamic.init_array.d_tag = bfdt_init_array;
    test->dynamic.init_array.d_ptr = offset(test->init_array, *test);
    test->dynamic.fini_array.d_tag = bfdt_fini_array;
    test->dynamic.fini_array.d_ptr = offset(test->fini_array, *test);
    test->dynamic.init_array.d_tag = bfdt_init_arraysz;
    test->dynamic.init_array.d_val = sizeof(test_init_array);
    test->dynamic.fini_array.d_tag = bfdt_fini_arraysz;
    test->dynamic.fini_array.d_val = sizeof(test_fini_array);
    test->dynamic.relacount.d_tag = bfdt_relacount;
    test->dynamic.relacount.d_val = 1;
    test->dynamic.flags_1.d_tag = bfdt_flags_1;

    test->shdrtab.eh_frame.sh_name = 0x0;
    test->shdrtab.eh_frame.sh_type = bfsht_strtab;
    test->shdrtab.eh_frame.sh_flags = 0;
    test->shdrtab.eh_frame.sh_addr = 0x250;
    test->shdrtab.eh_frame.sh_offset = offset(test->eh_frame, *test);
    test->shdrtab.eh_frame.sh_size = sizeof(test_eh_frame);
    test->shdrtab.eh_frame.sh_link = 0;
    test->shdrtab.eh_frame.sh_addralign = 1;
    test->shdrtab.eh_frame.sh_entsize = 0;

    // test.shdrtab.dynsym.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name1, test.shstrtab));
    // test.shdrtab.dynsym.sh_type = bfsht_dynsym;
    // test.shdrtab.dynsym.sh_flags = bfshf_a;
    // test.shdrtab.dynsym.sh_addr = 0x250;
    // test.shdrtab.dynsym.sh_offset = offset(test.dynsym, test);
    // test.shdrtab.dynsym.sh_size = sizeof(test_dynsym);
    // test.shdrtab.dynsym.sh_link = 2;
    // test.shdrtab.dynsym.sh_addralign = 8;
    // test.shdrtab.dynsym.sh_entsize = sizeof(bfelf_sym);

    // test.shdrtab.hashtab.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name2, test.shstrtab));
    // test.shdrtab.hashtab.sh_type = bfsht_hash;
    // test.shdrtab.hashtab.sh_flags = bfshf_a;
    // test.shdrtab.hashtab.sh_addr = 0x250;
    // test.shdrtab.hashtab.sh_offset = offset(test.hashtab, test);
    // test.shdrtab.hashtab.sh_size = sizeof(test_hashtab);
    // test.shdrtab.hashtab.sh_link = 0;
    // test.shdrtab.hashtab.sh_addralign = 8;
    // test.shdrtab.hashtab.sh_entsize = 0x4;

    // test.shdrtab.strtab.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name3, test.shstrtab));
    // test.shdrtab.strtab.sh_type = bfsht_strtab;
    // test.shdrtab.strtab.sh_flags = bfshf_a;
    // test.shdrtab.strtab.sh_addr = 0x250;
    // test.shdrtab.strtab.sh_offset = offset(test.strtab, test);
    // test.shdrtab.strtab.sh_size = sizeof(test_strtab);
    // test.shdrtab.strtab.sh_link = 0;
    // test.shdrtab.strtab.sh_addralign = 1;
    // test.shdrtab.strtab.sh_entsize = 0;

    // test.shdrtab.relatab1.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab1.sh_type = bfsht_rela;
    // test.shdrtab.relatab1.sh_flags = bfshf_ai;
    // test.shdrtab.relatab1.sh_addr = 0x250;
    // test.shdrtab.relatab1.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab1.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab1.sh_link = 0;
    // test.shdrtab.relatab1.sh_addralign = 8;
    // test.shdrtab.relatab1.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab2.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab2.sh_type = bfsht_rela;
    // test.shdrtab.relatab2.sh_flags = bfshf_ai;
    // test.shdrtab.relatab2.sh_addr = 0x250;
    // test.shdrtab.relatab2.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab2.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab2.sh_link = 0;
    // test.shdrtab.relatab2.sh_addralign = 8;
    // test.shdrtab.relatab2.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab3.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab3.sh_type = bfsht_rela;
    // test.shdrtab.relatab3.sh_flags = bfshf_ai;
    // test.shdrtab.relatab3.sh_addr = 0x250;
    // test.shdrtab.relatab3.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab3.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab3.sh_link = 0;
    // test.shdrtab.relatab3.sh_addralign = 8;
    // test.shdrtab.relatab3.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab4.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab4.sh_type = bfsht_rela;
    // test.shdrtab.relatab4.sh_flags = bfshf_ai;
    // test.shdrtab.relatab4.sh_addr = 0x250;
    // test.shdrtab.relatab4.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab4.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab4.sh_link = 0;
    // test.shdrtab.relatab4.sh_addralign = 8;
    // test.shdrtab.relatab4.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab5.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab5.sh_type = bfsht_rela;
    // test.shdrtab.relatab5.sh_flags = bfshf_ai;
    // test.shdrtab.relatab5.sh_addr = 0x250;
    // test.shdrtab.relatab5.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab5.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab5.sh_link = 0;
    // test.shdrtab.relatab5.sh_addralign = 8;
    // test.shdrtab.relatab5.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab6.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab6.sh_type = bfsht_rela;
    // test.shdrtab.relatab6.sh_flags = bfshf_ai;
    // test.shdrtab.relatab6.sh_addr = 0x250;
    // test.shdrtab.relatab6.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab6.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab6.sh_link = 0;
    // test.shdrtab.relatab6.sh_addralign = 8;
    // test.shdrtab.relatab6.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab7.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab7.sh_type = bfsht_rela;
    // test.shdrtab.relatab7.sh_flags = bfshf_ai;
    // test.shdrtab.relatab7.sh_addr = 0x250;
    // test.shdrtab.relatab7.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab7.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab7.sh_link = 0;
    // test.shdrtab.relatab7.sh_addralign = 8;
    // test.shdrtab.relatab7.sh_entsize = sizeof(bfelf_rela);

    // test.shdrtab.relatab8.sh_name = static_cast<bfelf64_word>(offset(test.shstrtab.name5, test.shstrtab));
    // test.shdrtab.relatab8.sh_type = bfsht_rela;
    // test.shdrtab.relatab8.sh_flags = bfshf_ai;
    // test.shdrtab.relatab8.sh_addr = 0x250;
    // test.shdrtab.relatab8.sh_offset = offset(test.relatab, test);
    // test.shdrtab.relatab8.sh_size = sizeof(test_relatab);
    // test.shdrtab.relatab8.sh_link = 0;
    // test.shdrtab.relatab8.sh_addralign = 8;
    // test.shdrtab.relatab8.sh_entsize = sizeof(bfelf_rela);

    // test.hashtab.nbucket = 2;
    // test.hashtab.nchain = 2;

    return {std::move(buff), size};
}

int
main(int argc, char *argv[])
{
    return RUN_ALL_TESTS(bfelf_loader_ut);
}
