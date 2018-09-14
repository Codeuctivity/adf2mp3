// ================================================================================================
// -*- C++ -*-
// File: adf2mp3.cpp
// Author: Guilherme R. Lampert
// Created on: 23/12/14
// Brief: GTA Vice City ADF to MP3 converter.
//
// This code is public domain. I claim no copyrights over it.
// No warranty is offered or implied; use it at your own risk.
//
// ================================================================================================

// Standard C++ includes:
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Standard C / POSIX includes:
#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>

namespace adf2mp3
{
	using ubyte = unsigned char;
	using ulong = unsigned long;

	//
	// The GTA Vice City ADF files are MP3 files that
	// had each byte XORed with this magic constant.
	// 34 is 22 in hexadecimal and 42 in octal...
	//
	// Not sure who figured this out, but I got this
	// info from the Xentax File Format Wiki.
	//
	const ubyte GtaMagic = 34;

	//
	// Process the input file in chunks of this size in bytes.
	// The chunk buffer is allocated dynamically (it is a std::vector)
	// so this can be a fairly large value.
	//
	const ulong ChunkSize = 8192;

	// ========================================================

	struct FileStats
	{
		ulong fileLength;   // File length in bytes.
		bool  isDirectory;  // True if the path provided pointed to a directory.
		bool  isNormalFile; // True is the path provided pointed to a file.
	};

	FileStats fileStatsForPath(const std::string & pathname)
	{
		errno = 0; // Clear the errno global, just in case.
		struct stat statBuf;

		if (stat(pathname.c_str(), &statBuf) != 0)
		{
			throw std::runtime_error("Path '" + pathname + "': " + std::string(std::strerror(errno)));
		}

		FileStats fs;
		fs.fileLength = static_cast<ulong>(statBuf.st_size);

#if _WIN32
		fs.isDirectory = (statBuf.st_mode & _S_IFDIR) ? true : false;
		fs.isNormalFile = (statBuf.st_mode & _S_IFREG) ? true : false;
#else // !_WIN32
		fs.isDirectory = S_ISDIR(statBuf.st_mode) ? true : false;
		fs.isNormalFile = S_ISREG(statBuf.st_mode) ? true : false;
#endif // _WIN32

		return fs;
	}

	std::string removeExtension(const std::string & filename)
	{
		const auto lastDot = filename.find_last_of('.');
		if (lastDot == std::string::npos)
		{
			return filename;
		}
		return filename.substr(0, lastDot);
	}

	void printHelpText(const char * programName)
	{
		std::cout << "\n";
		std::cout << "Usage:\n";
		std::cout << "$ " << programName << " <input_file> [output_file]\n";
		std::cout << "  Runs the tool normally. If the output filename is not provided\n" <<
			"  the input filename is used but the extension is replaced with '.mp3'.\n";

		std::cout << "\n";
		std::cout << "Usage:\n";
		std::cout << "$ " << programName << " --help | -h\n";
		std::cout << "  Prints this help text.\n";
		std::cout << "\n";
	}

	void handleCommandLine(int argc, const char * argv[],
		std::string & inFileName, std::string & outFileName,
		bool & printHelpOnly, bool & missingArgs)
	{
		if (argc < 2)
		{
			missingArgs = true;
			return;
		}

		if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0)
		{
			printHelpOnly = true;
			return;
		}

		// input_file + output_file
		if (argc >= 3)
		{
			inFileName = argv[1];
			outFileName = argv[2];
		}
		else // Just input_file
		{
			inFileName = argv[1];
			outFileName.clear();
		}

		printHelpOnly = false;
		missingArgs = false;
	}

	void processChunk(std::vector<ubyte> & chunk, ulong numBytes)
	{
		for (ulong b = 0; b < numBytes; ++b)
		{
			chunk[b] ^= GtaMagic;
		}
	}

	void processFiles(const std::string & inFileName, const std::string & outFileName)
	{
		const FileStats fileStats = fileStatsForPath(inFileName);

		if (fileStats.fileLength == 0)
		{
			throw std::runtime_error("Input file is empty!");
		}

		if (fileStats.isDirectory)
		{
			throw std::runtime_error("Input file '" + inFileName + "' is a directory!");
		}

		std::ifstream inFile(inFileName, std::ifstream::in | std::ifstream::binary);
		std::ofstream outFile(outFileName, std::ofstream::out | std::ofstream::binary);

		ulong bytesProcessed = 0;
		std::vector<ubyte> chunk(ChunkSize, 0);

		while (bytesProcessed != fileStats.fileLength)
		{
			const ulong bytesLeft = fileStats.fileLength - bytesProcessed;
			const ulong bytesThisIteration = std::min<ulong>(chunk.size(), bytesLeft);

			inFile.read(reinterpret_cast<char *>(chunk.data()), bytesThisIteration);
			processChunk(chunk, bytesThisIteration);
			outFile.write(reinterpret_cast<char *>(chunk.data()), bytesThisIteration);

			bytesProcessed += bytesThisIteration;
		}
	}

	void run(int argc, const char * argv[])
	{
		bool printHelpOnly, missingArgs;
		std::string inFileName, outFileName;
		handleCommandLine(argc, argv, inFileName, outFileName, printHelpOnly, missingArgs);

		if (printHelpOnly)
		{
			printHelpText(argv[0]);
			return;
		}

		if (missingArgs)
		{
			std::cout << "Not enough arguments!\n";
			printHelpText(argv[0]);
			return;
		}

		// Replace .adf extension of source file with .mp3 and use
		// it for the output if no explicit filename was provided.
		if (outFileName.empty())
		{
			outFileName = removeExtension(inFileName) + ".mp3";
		}

		processFiles(inFileName, outFileName);
	}
} // namespace adf2mp3 {}

int main(int argc, const char * argv[])
{
	try
	{
		adf2mp3::run(argc, argv);
		return 0;
	}
	catch (std::exception & e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}