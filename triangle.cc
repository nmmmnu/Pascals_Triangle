#include <cstdio>
#include <cstdint>

#include <vector>

constexpr size_t WIDTH	= 8;

using int_type = uint32_t;

using Line = std::vector<int_type>;

template<class PRINTER>
class Triangle{
public:
	Triangle(PRINTER &&printer, size_t const width) :
				printer(std::move(printer)),
				width(width),
				line1(width),
				line2(width){}

	void print(const Line &line, size_t const size) const{
		printer(line, size, width);
	}


	void calc(Line &line, const Line &oline, size_t const size){
		line[0] = oline[0];

		if (size > 0)
			for(size_t i = 1; i < size; ++i){
				line[i] = oline[i - 1] + oline[i];
			}

		line[size] = oline[size - 1];
	}

	void generate(){
		line1[0] = 1;
		print(line1, 0);

		for(size_t i = 1; i < width; ++i)
			if (i % 2 == 0){
				calc(line1, line2, i);
				print(line1, i);
			}else{
				calc(line2, line1, i);
				print(line2, i);
			}
	}

private:
	PRINTER		printer;

	size_t		width;

	Line		line1;
	Line		line2;
};



struct TextPrinter{
	void operator()(const Line &line, size_t const size, size_t const) const{
		for(size_t i = 0; i < size + 1; ++i)
			printf("%5u ", line[i]);

		printf("\n");
	}
};



struct TextPrinterPyramidal{
	void operator()(const Line &line, size_t const size, size_t const maxsize) const{
		_printSpaces(size, maxsize);

		for(size_t i = 0; i < size + 1; ++i){
			printf("%5u ", line[i]);
		}

		printf("\n");
	}

private:
	void _printSpaces(size_t const size, size_t const maxsize) const{
		size_t spaces = IWIDTH * (maxsize - size) / 2 ;

		for (size_t i = 0; i < spaces; ++i)
			printf(" ");


	}

private:
	constexpr static size_t IWIDTH = 5 + 1;
};



class ASCIIArtPrinterPyramidal{
public:
	ASCIIArtPrinterPyramidal(int_type modulus) :
				modulus(modulus){}

	void operator()(const Line &line, size_t const size, size_t const maxsize) const{
		_printSpaces(size, maxsize);

		for(size_t i = 0; i < size + 1; ++i){
			_printPattern(line[i]);
		}

		printf("\n");
	}

private:
	void _printPattern(int_type const value) const{
		char const c = ASCII_PATTERNS[ value % modulus ];

		printf("%c%c", c, c);
	}

	void _printSpaces(size_t const size, size_t const maxsize) const{
		size_t spaces = IWIDTH * (maxsize - size) / 2 ;

		for (size_t i = 0; i < spaces; ++i)
			printf(" ");
	}

private:
	constexpr static size_t IWIDTH = 2;

	constexpr static char ASCII_PATTERNS[] = { ' ', 'x', '+', '-', 'W' };
private:
	int_type	modulus;
};

constexpr char ASCIIArtPrinterPyramidal::ASCII_PATTERNS[];


class BitmapPrinter{
public:
	BitmapPrinter(int_type modulus) :
				modulus(modulus){}

	void operator()(const Line &line, size_t const size, size_t const maxsize) const{
		if (size == 0)
			_printHeader(maxsize * 2);

		for(size_t comp = 0; comp < 2; ++comp){
			_printSpaces(size, maxsize);

			for(size_t i = 0; i < size + 1; ++i){
				_printPattern(line[i]);
			}

			_printSpaces(size, maxsize);

			printf("\n");
		}
	}

private:
	void _printHeader(size_t const size) const{
		printf("P2\n");
		printf("%zu %zu\n", size + 2, size);
		printf("%d\n", modulus);
	}

	void _printPattern(int_type const value, bool const half = false) const{
		int_type  color = (int_type) ( value % modulus );

		if (color % 8 == 0)
			color = 0;

		if (half)
			printf("%3d ", color);
		else
			printf("%3d %3d ", color, color);
	}

	void _printSpaces(size_t const size, size_t const maxsize) const{
		size_t const spaces = (maxsize - size) / 2;

		//printf(">>%zu %zu %zu\n", size, maxsize, spaces);

		for (size_t i = 0; i < spaces; ++i)
			_printPattern(0);

		if ( (maxsize - size) % 2 )
			_printPattern(0, true);

	}

private:
	int_type	modulus;
};

#include <stdlib.h>

int main(int argc, char **argv){
	if (argc < 2){
		printf("Usage:\n");
		printf("\t%s [modulus] [size]\n", argv[0]);
		return 1;
	}

//	using Printer = TextPrinter;
//	using Printer = TextPrinterPyramidal;
//	using Printer = ASCIIArtPrinterPyramidal;
	using Printer = BitmapPrinter;

	long modulus = atol(argv[1]);
	if (modulus < 2)
		modulus = 2;

	long size = atol(argv[2]);
	if (size < 16)
		size = 16;

	Printer prn{ (int_type) modulus };

	Triangle<Printer> t(std::move(prn), (int_type) size);

	t.generate();
}

