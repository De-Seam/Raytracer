#pragma once
	template<typename T> class Vector
	{
	private:
		int maxsize, m_size;
	public:
		T* array;
		Vector();
		Vector(int i);
		~Vector();
		Vector(const Vector& other);
		Vector& operator = (const Vector& other)
		{
			if (&other != this)
			{
				m_size = other.m_size;
				maxsize = m_size;
				this->array = new T[m_size];
				for (int i = 0; i < m_size; i++)
					this->array[i] = other.array[i];
			}
			return *this;
		}
		int size() { return m_size; }
		void resize(int i);
		void push_back(T i);
		void aloc_new();
		void edit(int location, T i);
		void insert(int location, T i);
		void remove(int i);
		void reserve(int a);
		T& back();
		T& at(int i);
		T& operator[](int);
		T* data();
	};

	template<typename T>
	Vector<T>::~Vector()
	{
		delete[] array;
	}

	template<typename T>
	Vector<T>::Vector()
	{
		maxsize = 10;
		array = new T[maxsize];
		m_size = 0;
	}

	template<typename T>
	Vector<T>::Vector(const Vector& other)
	{
		m_size = other.m_size;
		maxsize = m_size;
		T* temp = new T[m_size];
		for (int i = 0; i < m_size; i++)
			temp[i] = other.array[i];
		//delete[] array;
		array = temp;
	}

	template<typename T>Vector<T>::Vector(int i)
	{
		maxsize = i;
		array = new T[maxsize];
		m_size = 0;
	}

	template<typename T>
	void Vector<T>::resize(int i)
	{
		if (i > m_size)
			while (i > maxsize)
				aloc_new();
		m_size = i;
	}

	template<typename T>
	void Vector<T>::remove(int i)
	{
		if (i > m_size)
			throw 10;
		else
		{
			for (int a = i + 1; a < m_size; a++)
			{
				array[a - 1] = array[a];
			}
			array[m_size - 1] = NULL;
			m_size--;
		}
	}

	template<typename T>
	void Vector<T>::push_back(T i)
	{
		if (m_size + 1 > maxsize)
			aloc_new();
		array[m_size] = i;
		m_size++;
	}

	template<typename T>
	void Vector<T>::edit(int location, T value)
	{
		array[location] = value;
	}

	template<typename T>
	void Vector<T>::insert(int location, T i)
	{
		if (m_size + 1 > maxsize)
			aloc_new();
		T* temp = new T[maxsize];
		for (int a = 0; a < m_size; a++)
			temp[a] = array[a];

		m_size++;
		for (int a = location + 1; a < m_size; a++)
		{
			array[a] = temp[a - 1];
		}
		array[location] = i;
	}

	template<typename T>
	void Vector<T>::aloc_new()
	{
		maxsize = m_size + 12;
		T* temp = new T[maxsize];
		for (int i = 0; i < m_size; i++)
			temp[i] = array[i];
		//delete[] array;
		array = temp;
	}

	template<typename T>
	void Vector<T>::reserve(int a)
	{
		maxsize = a+m_size;
		T* temp = new T[maxsize];
		for (int i = 0; i < m_size; i++)
			temp[i] = array[i];
		//for (int i = 0; i < maxsize - m_size; i++)
		//	temp[i] = nullptr;
		//m_size = maxsize;
		//delete[] array;
		array = temp;
	}

	template<typename T>
	T& Vector<T>::back()
	{
		if (m_size > 0)
			return array[m_size - 1];
		throw 10;
	}

	template<typename T>
	T& Vector<T>::at(int i)
	{
		if (i < m_size)
			return array[i];
		throw 10;
	}

	template<typename T>
	T& Vector<T>::operator[](int i)
	{
		if (i < m_size)
			return array[i];
		throw 10;
	}

	template<typename T>
	T* Vector<T>::data()
	{
		return __builtin_addressof(*array);
	}