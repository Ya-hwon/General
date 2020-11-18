#pragma once

#ifndef JComplex
#define JComplex

//#define _USE_MATH_DEFINES

#define gausseanMode 0b01	//rectangular / gaussean / cartesian
#define trigonoMode 0b010	//trigonometric / polar
#define eulersMode 0b0100
#define imaginaryUnit 'i'

#define stdOutputMode gausseanMode //setze standartausgabemodus
#define degreeMode	//undef for radian

#include <string>
#include <sstream>	//ein&ausgabeerweiterungen
#include <math.h>
#include <limits>
#include <vector>

#define conv(x) x	//zum unschalten zwischen radial und grad

#define makec(x) Complex(x, 0)

#define PolarToCartesian(x, y) Complex((x*cos(y)),(x*sin(y))) //abs, theta
#define ThetaOfC(x) (atan2(x.getImaginary(), x.getReal()))	//winkel von der positiven x-achse einer komplexen Zahl

#ifdef degreeMode
#undef conv(x)
#define conv(x) x*180/M_PI	//konvertierung zu grad
#define degreeModeB true
#else
#define degreeModeB false
#endif

namespace J {
	class Complex {
	private:
		long double real, imaginary;	//speicher f�r imagin�r und realanteil
	protected:
	public:
		Complex(long double real = 0L, long double imaginary = 0L) :real{ real }, imaginary{ imaginary }{}
		Complex(Complex& other) :real{ other.real }, imaginary{ other.imaginary }{}
		Complex(const Complex& other) :real{ other.real }, imaginary{ other.imaginary }{}
		Complex(std::string& str) {
				real = std::stod(str.substr(0, str.find(',')));
				imaginary = std::stod(str.substr(str.find(',') + 1, std::string::npos));
		}
		Complex(const std::string& str) {
				real = std::stod(str.substr(0, str.find(',')));
				imaginary = std::stod(str.substr(str.find(',') + 1, std::string::npos));
		}
		~Complex() {}

		long double getReal()const{return real;}
		long double getImaginary()const{return imaginary;}

		operator int() { if (imaginary==0); return (int)real; }
		operator float() { if (imaginary==0); return (float)real; }
		operator long double() { if (imaginary==0); return real; }

		bool isZero(){return(real==0)&&(imaginary==0);}

		long double abs()const{return(sqrt(pow(real,2)+pow(imaginary,2)));}

		Complex conjugate() {return {real, -imaginary};}

		std::string toString(int mode = stdOutputMode) const{
			if (mode & gausseanMode) { //ausgabe in koordinatenform
				std::ostringstream out;
				out.precision(std::numeric_limits<long double>::max_digits10);
				out << this->getReal() << (signbit(this->getImaginary())?'-':'+') << ::abs(this->getImaginary()) << imaginaryUnit;
				return out.str();
			}
			else if (mode & trigonoMode) { //ausgabe in polarer form
				long double theta = ThetaOfC((*this));	//winkel von realachse
				std::ostringstream out;
				out.precision(std::numeric_limits<long double>::max_digits10);
				out << this->abs() << "(cos(" << conv(theta) << (degreeModeB ? "deg" : "rad") << ")+" << imaginaryUnit << "sin(" << conv(theta) << (degreeModeB ? "deg" : "rad") << "))";
				return out.str();
				}
			else if (mode & eulersMode) { //ausgabe in eulers form (ae^itheta)
				std::ostringstream out;
				out.precision(std::numeric_limits<long double>::max_digits10);
				out << this->abs() <<"e^("  << conv(ThetaOfC((*this))) << (degreeModeB ? "deg*" : "rad*") << imaginaryUnit << ')';
				return out.str();
			}
			return"parameter malformed";//ung�ltiger parameter
		}
		bool operator==(Complex op1)const{return((op1.getImaginary()==this->getImaginary())&&(op1.getReal()==this->getReal()));}
		bool operator!=(Complex op1)const{return((op1.getImaginary()!=this->getImaginary())||(op1.getReal()!=this->getReal()));}

		//add divided by 0 exception
		Complex operator/(Complex op1)const { if (op1 == (Complex)0); double div = pow(op1.real, 2) + pow(op1.imaginary, 2); return{ (this->real * op1.real + this->imaginary * op1.imaginary) / div, (op1.real * this->imaginary - this->real * op1.imaginary) / div }; }
		Complex operator/(long double op1)const { if (!op1); return{ this->real / op1,op1 / this->imaginary }; }
		Complex operator*(Complex op1)const{return{op1.real*this->real-op1.imaginary*this->imaginary,op1.real*this->imaginary+op1.imaginary*this->real};}
		Complex operator*(long double op1)const{return{this->real*op1,op1*this->imaginary};}
		Complex operator+(Complex op1)const{return{this->real+op1.real,this->imaginary+op1.imaginary};}
		Complex operator+(long double op1)const{return{op1*this->real,this->imaginary};}
		Complex operator-(Complex op1)const{return{this->real-op1.real,this->imaginary-op1.imaginary};}
		Complex operator-(long double op1)const{return{this->real-op1,this->imaginary};}
	}const i{0,1};

	long double abs(const Complex& c){return(sqrt(pow(c.getReal(),2)+pow(c.getImaginary(),2)));}
	long double abs(Complex& c){return(sqrt(pow(c.getReal(),2)+pow(c.getImaginary(),2)));}

	long double Real(const Complex& other){return other.getReal();}
	long double Real(Complex& other){return other.getReal();}
	long double Imaginary(const Complex& other){return other.getImaginary();}
	long double Imaginary(Complex& other){return other.getImaginary();}

	Complex Conjugate(Complex c) { return {c.getReal(), -c.getImaginary()}; }

	Complex _cpow(long double op1, long double p) {
		return { makec(pow(op1, p)) };
	}
	Complex _cpow(Complex op1, long double p) {
		return PolarToCartesian(pow(op1.abs(), p), (ThetaOfC(op1) * p));
	}
	Complex _cpow(long double op1, Complex p) {
		return { pow(op1,p.getReal()) *cos(p.getImaginary()*log(op1)),pow(op1,p.getReal())*sin(p.getImaginary() * log(op1))};
	}
	Complex _cpow(Complex op1, Complex p) {//op1^p
		long double theta = ThetaOfC(op1);
		long double absm = abs(op1);
		long double skalar = exp(p.getReal() * log(absm) - p.getImaginary() * theta);
		return PolarToCartesian(skalar, p.getReal() * theta + p.getImaginary() * log(absm));
	}

	std::vector<Complex> _croot(Complex op1, Complex n) {
		return {_cpow(op1, makec(1)/n)};
	}
	std::vector<Complex> _croot(Complex op1, long double n) {
		std::vector<Complex> ret;
		if (floor(n) != n)return { _cpow(op1, 1/n) };
		long double theta = (ThetaOfC(op1) / n);
		long double absn = pow(op1.abs(), 1/n);
		ret.push_back( PolarToCartesian(absn, theta) );
		for (int i = 1; i < abs(n); i++) {
			theta += (2 * M_PI)/abs(n);
			ret.push_back(PolarToCartesian(absn, theta));
			//rotiere ergebnis n-1 mal um 360deg/n
		}
		return ret;
	}
	std::vector<Complex> _croot(long double op1, Complex n) {
		if (op1==0)return { makec(0) };
		return { _cpow(makec(op1), makec(1) / n) };
	}
	std::vector<Complex> _croot(long double op1, long double n) {
		std::vector<Complex> ret;
		if (floor(n) != n)return { _cpow(op1, 1 / n) };
		if (op1 == 0) {
			ret.push_back(makec(0));
		}
		else if (op1 > 0) {
			long double theta = 0;
			long double absn = pow(op1, 1 / n);
			ret.push_back(PolarToCartesian(absn, theta));
			for (int i = 1; i < abs(n); i++) {
				theta += (2 * M_PI) / abs(n);
				ret.push_back(PolarToCartesian(absn, theta));
				//rotiere ergebnis n-1 mal um 360deg/n
			}
		}
		else {
			return _croot(makec(op1), n);
		}
		return ret;
	}

	std::vector<Complex> _csqrt(Complex op1) {
		return _croot(op1, 2);
	}
	std::vector<Complex> _csqrt(long double op1) {
		if (op1 == 0)return { makec(0) };
		std::vector<Complex> ret;
		if (op1 > 0) {
			ret.push_back(makec(sqrt(op1)));
			ret.push_back(makec(-sqrt(op1)));
		}
		else if (op1 == 0) {
			ret.push_back(makec(0));
		}
		else {
			ret.push_back({ 0,sqrt(-op1) });
			ret.push_back({ 0,-sqrt(-op1) });
		}
		return ret;
	}


	std::vector<Complex> csqrt(Complex op1) {
		if (op1.getImaginary() == 0) {
			return _csqrt((long double)op1);
		}
		else {
			return _csqrt(op1);
		}
	}

	std::vector<Complex> croot(Complex op1, Complex op2) {
		if (op1.getImaginary() == 0 && op2.getImaginary() == 0) {
			return _croot((long double)op1, (long double)op2);
		}
		else if (op1.getImaginary() == 0) {
			return _croot((long double)op1, op2);
		}
		else if (op2.getImaginary() == 0) {
			return _croot(op1, (long double)op2);
		}
		else {
			return _croot(op1, op2);
		}
	}

	Complex cpow(Complex op1, Complex op2) {
		if (op1.getImaginary() == 0 && op2.getImaginary() == 0) {
			return _cpow((long double)op1, (long double)op2);
		}
		else if (op1.getImaginary() == 0) {
			return _cpow((long double)op1, op2);
		}
		else if (op2.getImaginary() == 0) {
			return _cpow(op1, (long double)op2);
		}
		else {
			return _cpow(op1, op2);
		}
	}

	Complex log(Complex& z) {
		return{log(z.abs()),ThetaOfC(z)};
	}
	Complex log(const Complex& z) {
		return{ log(z.abs()),ThetaOfC(z)};
	}

	std::ostream& operator<<(std::ostream& os, const Complex& c){ // operator zur ausgabe durch einen outputstream
		os << c.toString(stdOutputMode);
		return os;
	}

	Complex logn(Complex z, Complex n) {
		return { log(z) / log(n) };
	}
}

#endif
