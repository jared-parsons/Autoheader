#ifndef GUARD_Maybe_hpp
#define GUARD_Maybe_hpp

#include <algorithm>
#include <new>

template <class ValueType> class Maybe {
	//////////////////
	// Data Members //
	//////////////////
	bool _hasValue;
	alignas(ValueType) char _storage[sizeof(ValueType)];

	//////////////
	// Creation //
	//////////////
public:
	/*!
	Constructs a Maybe with no value.
	*/
	Maybe() {
		_hasValue = false;
	}

	/*!
	Constructs a Maybe with a value equal to \p value.
	*/
	Maybe(const ValueType &value) {
		_hasValue = true;
		new(_storage) ValueType(value);
	}

	/*!
	Constructs a Maybe with a value equal to \p value by moving.
	*/
	Maybe(ValueType &&value) {
		_hasValue = true;
		new(_storage) ValueType(std::move(value));
	}

	/////////////
	// Copying //
	/////////////
	/*!
	Copy constructs a Maybe from another Maybe.
	*/
	Maybe(const Maybe &copyFrom) {
		_hasValue = copyFrom._hasValue;
		if (copyFrom._hasValue)
			new(_storage) ValueType(copyFrom.GetValue());
	}

	/*!
	Copies from another Maybe.
	*/
	Maybe &operator=(const Maybe &copyFrom) {
		if (this != &copyFrom) {
			Maybe temporary(copyFrom);
			*this = std::move(copyFrom);
		}
		return *this;
	}

	////////////
	// Moving //
	////////////
private:
	void MoveFrom(Maybe &moveFrom) {
		_hasValue = moveFrom._hasValue;
		if (_hasValue) {
			new(_storage) ValueType(std::move(moveFrom.GetValue()));
			moveFrom.Destroy();
			moveFrom._hasValue = false;
		}
	}

public:
	/*!
	Move constructs a Maybe from another Maybe. \p moveFrom will not have a value after this returns.
	*/
	Maybe(Maybe &&moveFrom) noexcept {
		MoveFrom(moveFrom);
	}

	/*!
	Moves from another Maybe. \p moveFrom will not have a value after this returns.
	*/
	Maybe &operator=(Maybe &&moveFrom) noexcept {
		if (this != &moveFrom) {
			Destroy();
			MoveFrom(moveFrom);
		}
		return *this;
	}

	///////////
	// Value //
	///////////
	/*!
	\return \c true if this Maybe has a value, \c false otherwise.
	*/
	bool HasValue() const {
		return _hasValue;
	}

	/*!
	\return A mutable reference to the Maybe's value.
	\pre The Maybe must have a value.
	*/
	ValueType &GetValue() {
		// thang : we could assert(_hasValue)
		return *reinterpret_cast<ValueType *>(_storage);
	}

	/*!
	\return A non-mutable reference to the Maybe's value.
	\pre The Maybe must have a value.
	*/
	const ValueType &GetValue() const {
		// thang : we could assert(_hasValue)
		return *reinterpret_cast<const ValueType *>(_storage);
	}

	/////////////////
	// Destruction //
	/////////////////
private:
	void Destroy() noexcept {
		if (_hasValue)
			GetValue().~ValueType();
	}

public:
	/*!
	Destroys the Maybe's value, if it has one.
	*/
	~Maybe() {
		Destroy();
	}
};

#endif
