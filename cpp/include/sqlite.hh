#ifndef IVANP_SQLITE_HH
#define IVANP_SQLITE_HH

#include <sqlite3.h>
#include <exception>
#include <type_traits>
#include <cstring>

namespace ivanp {

namespace detail {
template <typename T>
using is_string_like = std::integral_constant<bool,
  std::is_convertible<
    decltype(std::declval<const T&>().data()), const char* >::value >;
template <typename S, typename T=void>
using enable_for_strings = std::enable_if_t< is_string_like<S>::value, T >;

const char* c_str(const char* s) noexcept { return s; }
template <typename T>
enable_for_strings<T,const char*> c_str(const T& s) noexcept { return s.data(); }
}

class sqlite {
  sqlite3 *db;

  struct error: std::exception {
  private:
    const char* msg;
  public:
    error(): msg(nullptr) { }
    error(const char* msg): msg(msg) { }
    error(const error&) = delete;
    error(error&& r) noexcept: msg(r.msg) { r.msg = nullptr; }
    error& operator=(const error&) = delete;
    error& operator=(error&& r) noexcept {
      msg = r.msg;
      r.msg = nullptr;
      return *this;
    }
    // ~error() { if (msg) sqlite3_free(msg); }
    const char* what() const noexcept { return msg; }
  };

  template <typename F>
  static int exec_callback(
    void* arg, // 4th argument of sqlite3_exec
    int ncol, // number of columns
    char** row, // pointers to strings obtained as if from sqlite3_column_text()
    char** cols_names // names of columns
  ) {
    (*reinterpret_cast<F*>(arg))(ncol,row,cols_names);
    return 0;
  }

public:
  const char* errmsg() const { return sqlite3_errmsg(db); }

  sqlite(const char* fname) {
    if (sqlite3_open(fname,&db) != SQLITE_OK)
      throw error(errmsg());
  }
  template <typename T, typename = detail::enable_for_strings<T>>
  sqlite(const T& fname): sqlite(static_cast<const char*>(fname.data())) { }
  ~sqlite() { sqlite3_close(db); }

  sqlite(const sqlite&) = delete;
  sqlite(sqlite&& r) noexcept: db(r.db) { r.db = nullptr; }
  sqlite& operator=(const sqlite&) = delete;
  sqlite& operator=(sqlite&& r) {
    db = r.db;
    r.db = nullptr;
    return *this;
  }

  template <typename T>
  sqlite& exec(const T& sql) {
    char* err;
    if (sqlite3_exec(db, detail::c_str(sql), nullptr, nullptr, &err)
        != SQLITE_OK) throw error(err);
    return *this;
  }
  template <typename T, typename F>
  sqlite& exec(const char* sql, F&& f) {
    char* err;
    if (sqlite3_exec( db, detail::c_str(sql),
          exec_callback<F>, reinterpret_cast<void*>(&f), &err
    ) != SQLITE_OK) throw error(err);
    return *this;
  }

  template <typename... T>
  sqlite& operator()(T&&... args) { return exec(std::forward<T>(args)...); }

  class stmt;

  class value {
    friend class stmt;
    sqlite3_value* p;
  public:
    value(): p(nullptr) { }
    value(sqlite3_value* p): p(sqlite3_value_dup(p)) { }
    ~value() { sqlite3_value_free(p); }
    value(const value& o) noexcept: p(sqlite3_value_dup(o.p)) { }
    value(value&& o) noexcept: p(o.p) { o.p = nullptr; }
    value& operator=(const value& o) noexcept {
      p = sqlite3_value_dup(o.p);
      return *this;
    }
    value& operator=(value&& o) noexcept {
      p = o.p;
      o.p = nullptr;
      return *this;
    }
    int type() const noexcept { return sqlite3_value_type(p); }
    int bytes() const noexcept { return sqlite3_value_bytes(p); }
    int as_int() const noexcept { return sqlite3_value_int(p); }
    int as_double() const noexcept { return sqlite3_value_double(p); }
    const void* as_blob() const noexcept { return sqlite3_value_blob(p); }
    const char* as_text() const noexcept {
      return reinterpret_cast<const char*>(sqlite3_value_text(p));
    }

    bool operator==(const value& o) const noexcept {
      auto t = type();
      if (t != o.type()) t = SQLITE_BLOB;
      switch (t) {
        case SQLITE_INTEGER: return as_int() == o.as_int();
        case SQLITE_FLOAT: return as_double() == o.as_double();
        case SQLITE_NULL: return true;
        default: { // TEXT or BLOB
          const auto len = bytes();
          if (o.bytes() != len) return false;
          return !memcmp(as_blob(),o.as_blob(),len);
        }
      }
    }
    bool operator<(const value& o) const noexcept {
      auto t = type();
      if (t != o.type()) t = SQLITE_BLOB;
      switch (t) {
        case SQLITE_INTEGER: return as_int() < o.as_int();
        case SQLITE_FLOAT: return as_double() < o.as_double();
        case SQLITE_NULL: return false;
        default: { // TEXT or BLOB
          const auto len = bytes();
          const auto len_o = o.bytes();
          auto cmp = memcmp(as_blob(),o.as_blob(),
            (len < len_o ? len : len_o) );
          return cmp ? (cmp < 0) : (len < len_o);
        }
      }
    }
    bool operator!=(const value& o) const noexcept { return !((*this) == o); }
  };

  class stmt {
    sqlite3_stmt *p = nullptr;

  public:
    stmt(sqlite3 *db, const char* sql) {
      if (sqlite3_prepare_v2(db, sql, -1, &p, nullptr) != SQLITE_OK)
        throw error(sqlite3_errmsg(db));
    }
    ~stmt() { sqlite3_finalize(p); }

    stmt(const stmt&) = delete;
    stmt(stmt&& r) noexcept: p(r.p) { r.p = nullptr; }
    stmt& operator=(const stmt&) = delete;
    stmt& operator=(stmt&& r) {
      p = r.p;
      r.p = nullptr;
      return *this;
    }

    sqlite3* db_handle() const { return sqlite3_db_handle(p); }
    const char* errmsg() const { return sqlite3_errmsg(db_handle()); }

    void finalize() {
      if (p) {
        const auto err = sqlite3_finalize(p);
        if (err != SQLITE_OK) throw error(sqlite3_errstr(err));
        p = nullptr;
      }
    }
    bool step() {
      switch (sqlite3_step(p)) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throw error(errmsg());
      }
    }
    stmt& reset() {
      if (sqlite3_reset(p) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }

    // bind ---------------------------------------------------------
    template <typename T>
    std::enable_if_t<std::is_floating_point<T>::value,stmt>&
    bind(int i, T x) {
      if (sqlite3_bind_double(p, i, x) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    template <typename T>
    std::enable_if_t<std::is_integral<T>::value && (sizeof(T)<=sizeof(int)),
    stmt>& bind(int i, T x) {
      if (sqlite3_bind_int(p, i, x) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    template <typename T>
    std::enable_if_t<std::is_integral<T>::value && (sizeof(T)>sizeof(int)),
    stmt>& bind(int i, T x) {
      if (sqlite3_bind_int64(p, i, x) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    stmt& bind(int i) {
      if (sqlite3_bind_null(p, i) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    stmt& bind(int i, const char* x, int n=-1, bool trans=true) {
      if (sqlite3_bind_text(p, i, x, n,
            trans ? SQLITE_TRANSIENT : SQLITE_STATIC
      ) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    stmt& bind(int i, const void* x, int n, bool trans=true) {
      if (sqlite3_bind_blob(p, i, x, n,
            trans ? SQLITE_TRANSIENT : SQLITE_STATIC
      ) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    stmt& bind(int i, std::nullptr_t, int n) {
      if (sqlite3_bind_zeroblob(p, i, n) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }
    stmt& bind(int i, const value& x) {
      if (sqlite3_bind_value(p, i, x.p) != SQLITE_OK)
        throw error(errmsg());
      return *this;
    }

    template <typename T>
    auto bind(int i, const T& x, int n=-1, bool trans=true)
    -> std::enable_if_t<
      std::is_convertible<decltype(x.data()),const char*>::value,
      stmt
    >& {
      return bind(i,static_cast<const char*>(x.data()),n,trans);
    }

  private:
    template <size_t I=0, typename T, typename... TT>
    void bind_row_impl(T&& x, TT&&... xx) {
      bind(I+1,std::forward<T>(x));
      bind_row_impl<I+1>(std::forward<TT>(xx)...);
    }
    template <size_t I=0, typename T>
    void bind_row_impl(T&& x) { bind(I+1,std::forward<T>(x)); }
  public:
    template <typename... T>
    stmt& bind_row(T&&... x) {
      bind_row_impl(std::forward<T>(x)...);
      step();
      return reset();
    }

    // column -------------------------------------------------------
    int column_count() {
      return sqlite3_column_count(p);
    }
    double column_double(int i) {
      return sqlite3_column_double(p, i);
    }
    int column_int(int i) {
      return sqlite3_column_int(p, i);
    }
    sqlite3_int64 column_int64(int i) {
      return sqlite3_column_int64(p, i);
    }
    const char* column_text(int i) {
      return reinterpret_cast<const char*>(sqlite3_column_text(p, i));
    }
    const void* column_blob(int i) {
      return sqlite3_column_blob(p, i);
    }
    value column_value(int i) {
      return sqlite3_column_value(p, i);
    }
    int column_bytes(int i) {
      return sqlite3_column_bytes(p, i);
    }
    int column_type(int i) {
      // SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL
      return sqlite3_column_type(p, i);
    }
    const char* column_name(int i) {
      return sqlite3_column_name(p, i);
    }
  };

  template <typename T>
  stmt prepare(const T& sql) { return { db, detail::c_str(sql) }; }

};

}

#endif
