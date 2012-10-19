

//BOOLEAN
#define BOOL int

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif



// VISIBLITY
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif




/*
int TcpSocket::CreateTcpAddressInfo(struct addrinfo **result,int port ,char* address)
{
	int iResult;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;//IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
		
	//Convert numeric port to string (for port/service)
	char buffer[6];
	_itoa_s(port,buffer, 10);
	//Resolve the local address and port to be used by the server
	iResult = getaddrinfo(address, buffer, &hints, result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		return -1;
	}
	return 0;
}
*/



/*
//A fixed-size array that contains the size specified at run-time.
//Note: Pretty much just want std::vector without being able to resize it and not initialize the values when created.
//Note: The problem with the new c++11 array class is that you must specify the size at compile-time.
template<typename T>
struct carray
{

	carray(size_t maxSize_) : data(new T[maxSize]), size(maxSize_), maxSize(maxSize_), isCreated(true) { }

	carray(T* data_, size_t maxSize_) : data(data_), size(maxSize_), maxSize(maxSize_), isCreated(false) { }

	~carray()
	{
		if(isCreated)
			delete data;
		return;
	}

	
	void SetSize(size_t size_) { assert(size <= maxSize); size = size_; }
	size_t Size() const { return size; }

	size_t MaxSize() const { return maxSize; }

	T* operator&() { return data; }
	const T* operator&() const { return data; }

	T& operator[](size_t i) { return data[i]; }
	const T& operator[](size_t i) const { return data[i]; }

	
  private:
	T* data;
	size_t size;
	const size_t maxSize;
	bool isCreated;

};


//A dynamic array that contains the size
template<typename T>
struct carray2d
{

	carray2d(size_t maxRows_, size_t maxColumns_)
		: data(new T[maxRows_][maxColumns_]),
		  rows(maxRows_), columns(maxColumns_),
		  maxRows(maxRows_), maxColumns(maxColumns_),
		  isCreated(true),
		  array1d(data, maxRows*maxColumns)
	{ }
	
	carray2d(T* data_, size_t maxRows_, size_t maxColumns_)
		: data(data_),
		  rows(maxRows_), columns(maxColumns_),
		  maxRows(maxRows_), maxColumns(maxColumns_),
		  isCreated(false)
		  array1d(data, maxRows*maxColumns)
	{ }

	~carray2d()
	{
		if(isCreated)
			delete data;
		return;
	}


	size_t Rows() const { return rows; }
	size_t Columns() const { return columns; }

	void SetRows(size_t rows_) { assert(rows <= maxRows); rows = rows_; }
	void SetColumns(size_t columns_) { assert(columns <= maxColumns); columns = columns_; }

	size_t MaxRows() const { return maxRows; }
	size_t MaxColumns() const { return maxColumns; }

	carray<T>& Flattened() const { return array1d; }

	T* operator&() { return data; }
	const T* operator&() const { return data; }

	T& operator()(size_t row, size_t col) { return data[row][col]; }
	const T& operator()(size_t row, size_t col) const { return data[row][col]; }

	//operator void*() { return (void*)data; }
	//operator const void*() const { return (const void*)data; }

  private:
	T* data;
	size_t rows;
	size_t columns;
	size_t maxRows;
	size_t maxColumns;
	bool isCreated;

	carray<T> array1d;

};
*/
