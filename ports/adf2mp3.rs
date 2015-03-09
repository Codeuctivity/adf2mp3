
// ================================================================================================
// -*- Rust -*-
// File: adf2mp3.rs
// Author: Guilherme R. Lampert
// Created on: 08/03/15
// Brief: GTA Vice City ADF to MP3 converter (Rust version of the original C++ code).
//
// This code is public domain. I claim no copyrights over it.
// No warranty is offered or implied; use it at your own risk.
//
// ================================================================================================

// Currently (rustc 1.0.0-nightly), these modules are all unstable
// and would generate warnings without these suppressions.
#![feature(io)]
#![feature(path)]
#![feature(collections)]

use std::fs::File;
use std::fs::Metadata;
use std::path::Path;
use std::io::prelude::*;

// ========================================================

// The GTA Vice City ADF files are MP3 files that
// had each byte XORed with this magic constant.
// 34 is 22 in hexadecimal and 42 in octal...
//
// Not sure who figured this out, but I got this
// info from the Xentax File Format Wiki.
//
const GTA_MAGIC: u8 = 34;

// Process the input file in chunks of this size in bytes.
// The chunk buffer is allocated dynamically (it is a Vec)
// so this can be a fairly large value.
//
const CHUNK_SIZE: usize = 8192;

// ========================================================

fn print_help_text(program_name: &str) {
	println!("");
	println!("Usage:");
	println!("$ {} <input_file> [output_file]", program_name);
	println!("  Runs the tool normally. If the output filename is not provided");
	println!("  the input filename is used but the extension is replaced with '.mp3'.");
	println!("");
	println!("Usage:");
	println!("$ {} --help | -h", program_name);
	println!("  Prints this help text.");
	println!("");
}

fn remove_extension(filename: &str) -> String {
	String::from_str(Path::new(filename).file_stem()
		.unwrap().to_str().unwrap())
}

fn open_file(filename: &str) -> (File, Metadata) {
	let path = Path::new(filename);

	// `open()` is read-only by default.
	let file_handle = match File::open(&path) {
		Err(why) => panic!("Couldn't open file {}: {}", path.display(), why.description()),
		Ok(file_handle) => file_handle,
	};

	let file_info = match file_handle.metadata() {
		Err(why) => panic!("Unable to get metadata for {}: {}", path.display(), why.description()),
		Ok(file_info) => file_info,
	};

	// Return a pair of file handle and its metadata:
	(file_handle, file_info)
}

fn create_file(filename: &str) -> File {
	let path = Path::new(filename);

	// `create()` is write-only by default.
	match File::create(&path) {
		Err(why) => panic!("Couldn't create file {}: {}", path.display(), why.description()),
		Ok(file_handle) => file_handle,
	}
}

fn process_files(input_filename: &str, output_filename: &str) {
	// Source file (ADF):
	let in_file_tuple = open_file(input_filename);
	let in_file_len   = in_file_tuple.1.len() as usize; // Index 1: Metadata
	let mut in_file   = in_file_tuple.0;                // Index 0: File

	// Destination file (MP3):
	let mut out_file = create_file(output_filename);

	// Chunk processing loop:
	let mut bytes_processed: usize = 0;
	while bytes_processed != in_file_len {
		let bytes_left = in_file_len - bytes_processed;
		let bytes_this_iteration = std::cmp::min(CHUNK_SIZE, bytes_left);

		let mut chunk = vec![0u8; bytes_this_iteration];

		match in_file.read(chunk.as_mut_slice()) {
			Err(why) => panic!("Failed to read input file! {}", why.description()),
			Ok(_) => {},
		};

		for byte in &mut chunk {
			*byte ^= GTA_MAGIC;
		}

		match out_file.write(chunk.as_mut_slice()) {
			Err(why) => panic!("Failed to write output file! {}", why.description()),
			Ok(_) => {},
		};

		bytes_processed += bytes_this_iteration;
	}
}

fn main() {
	// Fetch command line:
	let cmd_args: Vec<_> =
		std::env::args()
		.map(|x| x.to_string())
		.collect();

	// Too few command line args, exit:
	if cmd_args.len() < 2 {
		println!("Not enough arguments!");
		print_help_text(&cmd_args[0]);
		return;
	}

	// Print help and exit if "-h" or "--help" present in cmd line:
	if cmd_args.len() > 1 && (cmd_args[1] == "-h" || cmd_args[1] == "--help") {
		print_help_text(&cmd_args[0]);
		return;
	}

	//
	// Normal execution path:
	//
	let input_filename;
	let mut output_filename;

	if cmd_args.len() >= 3 {
		// input_filename + output_filename:
		input_filename  = cmd_args[1].clone();
		output_filename = cmd_args[2].clone();
	} else {
		// Just input_filename:
		input_filename  = cmd_args[1].clone();
		output_filename = String::new();
	}

	// Replace ".adf" extension of source filename with ".mp3" and use
	// it for the output if no explicit filename was provided.
	if output_filename.is_empty() {
		output_filename = remove_extension(&input_filename) + ".mp3";
	}

	process_files(&input_filename, &output_filename);
}
