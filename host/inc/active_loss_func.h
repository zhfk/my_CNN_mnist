#ifndef _ACTIVE_H_
#define _ACTIVE_H_
#include <algorithm>
#include <math.h>
namespace ActiveFun{
	template<typename T>
	class sigmoid{
	public:
		static T f(const T v)  { return T(1) / (T(1) + exp(-v)); }
		static T df(const T y)  { return y * (T(1) - y); }
		std::pair<T, T> scale()  { return std::make_pair(T(0.1), T(0.9)); }
	};

	template<typename T>
	class relu{
	public:
		static T f(const T v)  { return std::max(T(0), v); }
		static T df(T y)  { return y > T(0) ? T(1) : T(0); }
		std::pair<T, T> scale()  { return std::make_pair(T(0.1), T(0.9)); }
	};

	template<typename T>
	class tan_h{
	public:
		static T f(const T v)   {
			const T ep = exp(v);
			const T em = exp(-v);
			return (ep - em) / (ep + em);
		}
		static T df(T y)  { return T(1) - (y*y); }
		std::pair<T, T> scale()  { return std::make_pair(T(-0.8), T(0.8)); }
	};
}//end ActiveFun
namespace LossFun{
// mean-squared-error loss function for regression
	template<typename T>
	class mse {
	public:
		static T f(const T y, const T t) {
			return (y - t) * (y - t);
		}

		static T df(const T y, const T t) {
		//	T factor = T(2) / static_cast<T>(10);
			return y-t;
		}
	};
	// cross-entropy loss function for (multiple independent) binary classifications
	template<typename T>
	class cross_entropy {
	public:
	    static T f(const T& y, const T& t) {
	        assert(y.size() == t.size());
	        T d = T(0);
	        for(int i = 0; i < y.size(); ++i)
	            d += -t[i] * log(y[i]) - (T(1) - t[i]) * log(T(1) - y[i]);
	        return d;
	    }

	    static T df(const T& y, const T& t) {
	        assert(y.size() == t.size());
	        T d(t.size());
	        for(int i = 0; i < y.size(); ++i)
	            d[i] = (y[i] - t[i]) / (y[i] * (float_t(1) - y[i]));
	        return d;
	    }
	};
	template<typename Loss,typename T>
	T Loss_gradients(T y,T v)
	{
		return Loss::df(y,v);
	}
}//end namespace LossFun

#endif
