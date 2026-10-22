#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template sa_nracanaa HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	ប្រសិនបើ (ready())
		ត្រឡប់;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	សម្រាប់ (const var_na *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { ត្រឡប់ nodes_[idx]; }
bool StaticTwines::ready() { ត្រឡប់ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
ចំនួនគត់ twine_gc_count;

std::string IdString::handle_token() const {
	ត្រឡប់ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	ប្រសិនបើ (token.substr(0, 5) == "$pub@") {
		is_public = true;
		ត្រឡប់ 5;
	}
	ប្រសិនបើ (token.substr(0, 6) == "$priv@") {
		is_public = false;
		ត្រឡប់ 6;
	}
	ត្រឡប់ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ត្រឡប់ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ត្រឡប់ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	ប្រសិនបើ (len_ > INLINE_CAP) {
		ptr_ = new var_na[len_];
		memcpy(ptr_, content.data(), len_);
	} បើមិនដូច្នេះ {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	ប្រសិនបើ (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(const SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(const SmallString &other)
{
	ប្រសិនបើ (this != &other)
		store(other.view());
	ត្រឡប់ *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	ប្រសិនបើ (this == &other)
		ត្រឡប់ *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ត្រឡប់ *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	ប្រសិនបើ (this == &other)
		ត្រឡប់ *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ត្រឡប់ *this;
}

std::string TwineSpec::content_str() const {
	ប្រសិនបើ (auto *leaf = std::get_if<Leaf>(&data))
		ត្រឡប់ leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ត្រឡប់ *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	ប្រសិនបើ (!name.empty() && name[0] == '\\')
		ត្រឡប់ {name.substr(1), true};
	ត្រឡប់ {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	សម្រាប់ (var_na ch : name)
		ប្រសិនបើ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	ប្រសិនបើ (this == &other)
		ត្រឡប់ *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ត្រឡប់ *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	ប្រសិនបើ (this == &other)
		ត្រឡប់ *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ត្រឡប់ *this;
}

bool TwinePool::owns(IdString ref) const {
	ត្រឡប់ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	ប្រសិនបើ (ref == IdString::Null || ID::is_static(ref))
		ត្រឡប់ ref;
	ត្រឡប់ ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	ប្រសិនបើ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	ត្រឡប់ HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	សម្រាប់ (auto &it : auto_prefixes)
		roots.insert(it.second);
	ត្រឡប់ HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { ត្រឡប់ StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	ប្រសិនបើ constexpr (IdString::MAX_SERIAL == 0)
		ត្រឡប់ serial;
	បើមិនដូច្នេះ
		ត្រឡប់ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	ប្រសិនបើ (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	ត្រឡប់ HashConsPool::add_inner(std::move(t));
}

void TwinePool::dump(IdString ref, std::ostream& os) const {
	const TwineNode& twine = (*this)[ref];
	switch (twine.kind()) {
	pak_sa TwineNode::Kind::Dead:
		os << "Dead()";
		bha_nga;
	pak_sa TwineNode::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		bha_nga;
	pak_sa TwineNode::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		bha_nga;
	}
	ប្រសិនបើ (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	ប្រសិនបើ (ref == IdString::Null)
		ត្រឡប់;
	ប្រសិនបើ (ref.isPublic())
		out += '\\';
	const TwineNode& twine = (*this)[ref];
	switch (twine.kind()) {
	pak_sa TwineNode::Kind::Dead:
		bha_nga;
	pak_sa TwineNode::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	pak_sa TwineNode::Kind::Leaf:
		out += twine.text();
		bha_nga;
	}
}

std::string TwinePool::str(IdString ref) const {
	std::string out;
	append_str(ref, out);
	ត្រឡប់ out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	ត្រឡប់ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	ត្រឡប់ TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	ត្រឡប់ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	ប្រសិនបើ (ref != IdString::Null)
		ត្រឡប់ ref;
	ត្រឡប់ add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	ប្រសិនបើ (name.empty())
		ត្រឡប់ IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ត្រឡប់ stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	ប្រសិនបើ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ត្រឡប់ stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	ប្រសិនបើ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ត្រឡប់ stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ត្រឡប់ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	ប្រសិនបើ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ត្រឡប់ add(auto_prefix(ap->prefix), ap->tail);
	ប្រសិនបើ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ត្រឡប់ stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ត្រឡប់ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	ប្រសិនបើ (it != auto_prefixes.end())
		ត្រឡប់ it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ត្រឡប់ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ត្រឡប់ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	ប្រសិនបើ (s.empty())
		ត្រឡប់ IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ត្រឡប់ stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	ប្រសិនបើ (found != IdString::Null || ref == IdString::Null)
		ត្រឡប់ found;
	const TwineNode& t = src[ref.untag()];
	ប្រសិនបើ (t.is_dead())
		ត្រឡប់ IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	ប្រសិនបើ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ត្រឡប់ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	ត្រឡប់ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	ប្រសិនបើ (token.size() < 2 || token[0] != '#')
		ត្រឡប់ IdString::Null;
	size_t sep = token.find(':');
	ប្រសិនបើ (sep == std::string_view::npos)
		ត្រឡប់ IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	សម្រាប់ (ចំនួនគត់ i = 0; i < 2; i++) {
		ប្រសិនបើ (parts[i].empty())
			ត្រឡប់ IdString::Null;
		សម្រាប់ (var_na c : parts[i]) {
			ប្រសិនបើ (c < '0' || c > '9')
				ត្រឡប់ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ប្រសិនបើ (fields[0] != serial_)
		ត្រឡប់ IdString::Null;
	IdString ref(fields[1]);
	ប្រសិនបើ (ref == IdString::Null)
		ត្រឡប់ IdString::Null;
	ត្រឡប់ is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
sa_nracanaa TwineSegments {
	static constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(const TwinePool &pool, IdString ref);
	TwineSegments(const TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	void advance(size_t n);
	size_t total_size() const;
private:
	void push(std::string_view seg);
	std::string_view *segs();
	const std::string_view *segs() const;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t count = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(const TwinePool &pool, IdString ref)
{
	ប្រសិនបើ (ref == IdString::Null)
		ត្រឡប់;
	សម្រាប់ (IdString cur = ref.untag(); ;) {
		const TwineNode &t = pool[cur];
		switch (t.kind()) {
		pak_sa TwineNode::Kind::Suffix:
			push(t.text());
			cur = t.prefix();
			continue;
		pak_sa TwineNode::Kind::Leaf:
			push(t.text());
			bha_nga;
		pak_sa TwineNode::Kind::Dead:
			bha_nga;
		}
		bha_nga;
	}
	ប្រសិនបើ (ref.isPublic())
		push("\\");
	std::reverse(segs(), segs() + count);
}

TwineSegments::TwineSegments(const TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	push(tail);
}

std::string_view TwineSegments::peek()
{
	ខណៈពេល (pos < count) {
		std::string_view seg = segs()[pos];
		ប្រសិនបើ (off < seg.size())
			ត្រឡប់ seg.substr(off);
		pos++;
		off = 0;
	}
	ត្រឡប់ {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	សម្រាប់ (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	ត្រឡប់ total;
}

void TwineSegments::push(std::string_view seg)
{
	ប្រសិនបើ (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		ត្រឡប់;
	}
	ប្រសិនបើ (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { ត្រឡប់ spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { ត្រឡប់ spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	ត្រឡប់ TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	ខណៈពេល (!prefix.empty()) {
		std::string_view seg = segs.peek();
		ប្រសិនបើ (seg.empty())
			ត្រឡប់ false;
		size_t n = std::min(seg.size(), prefix.size());
		ប្រសិនបើ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ត្រឡប់ false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ត្រឡប់ true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	ត្រឡប់ str_size(ref) == name.size() && begins_with(ref, name);
}

static ចំនួនគត់ compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ខណៈពេល (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		ប្រសិនបើ (x.empty() || y.empty())
			ត្រឡប់ x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		ប្រសិនបើ (ចំនួនគត់ diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ត្រឡប់ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	ត្រឡប់ find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		ប្រសិនបើ (t == TwineNode::Key{prefix, text})
			ត្រឡប់ true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		ត្រឡប់ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	ប្រសិនបើ (ref == IdString::Null || ID::is_static(ref))
		ត្រឡប់ ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	ប្រសិនបើ (t.is_dead())
		ត្រឡប់ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		ត្រឡប់ compare_segments(sa, sb) == 0;
	});
	ត្រឡប់ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	ប្រសិនបើ (!t.is_suffix())
		ត្រឡប់ IdString::Null;
	ត្រឡប់ stamp(t.prefix().tag(ref.isPublic()));
}

ចំនួនគត់ TwinePool::compare_by_name(IdString a, IdString b) const
{
	ប្រសិនបើ (a == b)
		ត្រឡប់ 0;
	ប្រសិនបើ (a == IdString::Null)
		ត្រឡប់ -1;
	ប្រសិនបើ (b == IdString::Null)
		ត្រឡប់ 1;

	ប្រសិនបើ (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		ប្រសិនបើ (ta.is_leaf() && tb.is_leaf())
			ត្រឡប់ ta.text().compare(tb.text());
		ប្រសិនបើ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ត្រឡប់ ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	ត្រឡប់ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
