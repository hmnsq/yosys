#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template sa_nracanaa HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	གལ་ཏེ (ready())
		ཕྱིར;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	ཆེད་དུ (const var_na *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { ཕྱིར nodes_[idx]; }
bool StaticTwines::ready() { ཕྱིར nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
ཧྲིལ twine_gc_count;

std::string IdString::handle_token() const {
	ཕྱིར stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	གལ་ཏེ (token.substr(0, 5) == "$pub@") {
		is_public = true;
		ཕྱིར 5;
	}
	གལ་ཏེ (token.substr(0, 6) == "$priv@") {
		is_public = false;
		ཕྱིར 6;
	}
	ཕྱིར 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ཕྱིར result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ཕྱིར static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	གལ་ཏེ (len_ > INLINE_CAP) {
		ptr_ = new var_na[len_];
		memcpy(ptr_, content.data(), len_);
	} དེ་མིན {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	གལ་ཏེ (len_ > INLINE_CAP)
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
	གལ་ཏེ (this != &other)
		store(other.view());
	ཕྱིར *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	གལ་ཏེ (this == &other)
		ཕྱིར *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ཕྱིར *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	གལ་ཏེ (this == &other)
		ཕྱིར *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ཕྱིར *this;
}

std::string TwineSpec::content_str() const {
	གལ་ཏེ (auto *leaf = std::get_if<Leaf>(&data))
		ཕྱིར leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ཕྱིར *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	གལ་ཏེ (!name.empty() && name[0] == '\\')
		ཕྱིར {name.substr(1), true};
	ཕྱིར {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	ཆེད་དུ (var_na ch : name)
		གལ་ཏེ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	གལ་ཏེ (this == &other)
		ཕྱིར *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ཕྱིར *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	གལ་ཏེ (this == &other)
		ཕྱིར *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ཕྱིར *this;
}

bool TwinePool::owns(IdString ref) const {
	ཕྱིར ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	གལ་ཏེ (ref == IdString::Null || ID::is_static(ref))
		ཕྱིར ref;
	ཕྱིར ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	གལ་ཏེ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	ཕྱིར HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	ཆེད་དུ (auto &it : auto_prefixes)
		roots.insert(it.second);
	ཕྱིར HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { ཕྱིར StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	གལ་ཏེ constexpr (IdString::MAX_SERIAL == 0)
		ཕྱིར serial;
	དེ་མིན
		ཕྱིར serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	གལ་ཏེ (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	ཕྱིར HashConsPool::add_inner(std::move(t));
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
	གལ་ཏེ (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	གལ་ཏེ (ref == IdString::Null)
		ཕྱིར;
	གལ་ཏེ (ref.isPublic())
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
	ཕྱིར out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	ཕྱིར str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	ཕྱིར TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	ཕྱིར find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	གལ་ཏེ (ref != IdString::Null)
		ཕྱིར ref;
	ཕྱིར add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	གལ་ཏེ (name.empty())
		ཕྱིར IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ཕྱིར stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	གལ་ཏེ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ཕྱིར stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	གལ་ཏེ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ཕྱིར stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ཕྱིར stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	གལ་ཏེ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ཕྱིར add(auto_prefix(ap->prefix), ap->tail);
	གལ་ཏེ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ཕྱིར stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ཕྱིར add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	གལ་ཏེ (it != auto_prefixes.end())
		ཕྱིར it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ཕྱིར ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ཕྱིར stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	གལ་ཏེ (s.empty())
		ཕྱིར IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ཕྱིར stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	གལ་ཏེ (found != IdString::Null || ref == IdString::Null)
		ཕྱིར found;
	const TwineNode& t = src[ref.untag()];
	གལ་ཏེ (t.is_dead())
		ཕྱིར IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	གལ་ཏེ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ཕྱིར stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	ཕྱིར "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	གལ་ཏེ (token.size() < 2 || token[0] != '#')
		ཕྱིར IdString::Null;
	size_t sep = token.find(':');
	གལ་ཏེ (sep == std::string_view::npos)
		ཕྱིར IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	ཆེད་དུ (ཧྲིལ i = 0; i < 2; i++) {
		གལ་ཏེ (parts[i].empty())
			ཕྱིར IdString::Null;
		ཆེད་དུ (var_na c : parts[i]) {
			གལ་ཏེ (c < '0' || c > '9')
				ཕྱིར IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	གལ་ཏེ (fields[0] != serial_)
		ཕྱིར IdString::Null;
	IdString ref(fields[1]);
	གལ་ཏེ (ref == IdString::Null)
		ཕྱིར IdString::Null;
	ཕྱིར is_live(ref) ? ref : IdString::Null;
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
	གལ་ཏེ (ref == IdString::Null)
		ཕྱིར;
	ཆེད་དུ (IdString cur = ref.untag(); ;) {
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
	གལ་ཏེ (ref.isPublic())
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
	སྐབས (pos < count) {
		std::string_view seg = segs()[pos];
		གལ་ཏེ (off < seg.size())
			ཕྱིར seg.substr(off);
		pos++;
		off = 0;
	}
	ཕྱིར {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	ཆེད་དུ (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	ཕྱིར total;
}

void TwineSegments::push(std::string_view seg)
{
	གལ་ཏེ (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		ཕྱིར;
	}
	གལ་ཏེ (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { ཕྱིར spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { ཕྱིར spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	ཕྱིར TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	སྐབས (!prefix.empty()) {
		std::string_view seg = segs.peek();
		གལ་ཏེ (seg.empty())
			ཕྱིར false;
		size_t n = std::min(seg.size(), prefix.size());
		གལ་ཏེ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ཕྱིར false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ཕྱིར true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	ཕྱིར str_size(ref) == name.size() && begins_with(ref, name);
}

static ཧྲིལ compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	སྐབས (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		གལ་ཏེ (x.empty() || y.empty())
			ཕྱིར x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		གལ་ཏེ (ཧྲིལ diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ཕྱིར diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	ཕྱིར find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		གལ་ཏེ (t == TwineNode::Key{prefix, text})
			ཕྱིར true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		ཕྱིར compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	གལ་ཏེ (ref == IdString::Null || ID::is_static(ref))
		ཕྱིར ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	གལ་ཏེ (t.is_dead())
		ཕྱིར IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		ཕྱིར compare_segments(sa, sb) == 0;
	});
	ཕྱིར stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	གལ་ཏེ (!t.is_suffix())
		ཕྱིར IdString::Null;
	ཕྱིར stamp(t.prefix().tag(ref.isPublic()));
}

ཧྲིལ TwinePool::compare_by_name(IdString a, IdString b) const
{
	གལ་ཏེ (a == b)
		ཕྱིར 0;
	གལ་ཏེ (a == IdString::Null)
		ཕྱིར -1;
	གལ་ཏེ (b == IdString::Null)
		ཕྱིར 1;

	གལ་ཏེ (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		གལ་ཏེ (ta.is_leaf() && tb.is_leaf())
			ཕྱིར ta.text().compare(tb.text());
		གལ་ཏེ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ཕྱིར ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	ཕྱིར compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
