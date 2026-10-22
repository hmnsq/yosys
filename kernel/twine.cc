#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template struct HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	만약 (ready())
		반환;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	위해 (const char *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { 반환 nodes_[idx]; }
bool StaticTwines::ready() { 반환 nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
정수 twine_gc_count;

std::string IdString::handle_token() const {
	반환 stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	만약 (token.substr(0, 5) == "$pub@") {
		is_public = true;
		반환 5;
	}
	만약 (token.substr(0, 6) == "$priv@") {
		is_public = false;
		반환 6;
	}
	반환 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	반환 result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	반환 static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	만약 (len_ > INLINE_CAP) {
		ptr_ = new char[len_];
		memcpy(ptr_, content.data(), len_);
	} 아니면 {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	만약 (len_ > INLINE_CAP)
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
	만약 (this != &other)
		store(other.view());
	반환 *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	만약 (this == &other)
		반환 *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	반환 *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	만약 (this == &other)
		반환 *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	반환 *this;
}

std::string TwineSpec::content_str() const {
	만약 (auto *leaf = std::get_if<Leaf>(&data))
		반환 leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	반환 *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	만약 (!name.empty() && name[0] == '\\')
		반환 {name.substr(1), true};
	반환 {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	위해 (char ch : name)
		만약 ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	만약 (this == &other)
		반환 *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	반환 *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	만약 (this == &other)
		반환 *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	반환 *this;
}

bool TwinePool::owns(IdString ref) const {
	반환 ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	만약 (ref == IdString::Null || ID::is_static(ref))
		반환 ref;
	반환 ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	만약 constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	반환 HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	위해 (auto &it : auto_prefixes)
		roots.insert(it.second);
	반환 HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { 반환 StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	만약 constexpr (IdString::MAX_SERIAL == 0)
		반환 serial;
	아니면
		반환 serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	만약 (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	반환 HashConsPool::add_inner(std::move(t));
}

void TwinePool::dump(IdString ref, std::ostream& os) const {
	const TwineNode& twine = (*this)[ref];
	switch (twine.kind()) {
	case TwineNode::Kind::Dead:
		os << "Dead()";
		break;
	case TwineNode::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		break;
	case TwineNode::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		break;
	}
	만약 (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	만약 (ref == IdString::Null)
		반환;
	만약 (ref.isPublic())
		out += '\\';
	const TwineNode& twine = (*this)[ref];
	switch (twine.kind()) {
	case TwineNode::Kind::Dead:
		break;
	case TwineNode::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	case TwineNode::Kind::Leaf:
		out += twine.text();
		break;
	}
}

std::string TwinePool::str(IdString ref) const {
	std::string out;
	append_str(ref, out);
	반환 out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	반환 str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	반환 TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	반환 find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	만약 (ref != IdString::Null)
		반환 ref;
	반환 add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	만약 (name.empty())
		반환 IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	반환 stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	만약 (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		반환 stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	만약 (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		반환 stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	반환 stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	만약 (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		반환 add(auto_prefix(ap->prefix), ap->tail);
	만약 (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		반환 stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	반환 add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	만약 (it != auto_prefixes.end())
		반환 it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	반환 ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	반환 stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	만약 (s.empty())
		반환 IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	반환 stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	만약 (found != IdString::Null || ref == IdString::Null)
		반환 found;
	const TwineNode& t = src[ref.untag()];
	만약 (t.is_dead())
		반환 IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	만약 (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	반환 stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	반환 "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	만약 (token.size() < 2 || token[0] != '#')
		반환 IdString::Null;
	size_t sep = token.find(':');
	만약 (sep == std::string_view::npos)
		반환 IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	위해 (정수 i = 0; i < 2; i++) {
		만약 (parts[i].empty())
			반환 IdString::Null;
		위해 (char c : parts[i]) {
			만약 (c < '0' || c > '9')
				반환 IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	만약 (fields[0] != serial_)
		반환 IdString::Null;
	IdString ref(fields[1]);
	만약 (ref == IdString::Null)
		반환 IdString::Null;
	반환 is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
struct TwineSegments {
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
	만약 (ref == IdString::Null)
		반환;
	위해 (IdString cur = ref.untag(); ;) {
		const TwineNode &t = pool[cur];
		switch (t.kind()) {
		case TwineNode::Kind::Suffix:
			push(t.text());
			cur = t.prefix();
			continue;
		case TwineNode::Kind::Leaf:
			push(t.text());
			break;
		case TwineNode::Kind::Dead:
			break;
		}
		break;
	}
	만약 (ref.isPublic())
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
	동안 (pos < count) {
		std::string_view seg = segs()[pos];
		만약 (off < seg.size())
			반환 seg.substr(off);
		pos++;
		off = 0;
	}
	반환 {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	위해 (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	반환 total;
}

void TwineSegments::push(std::string_view seg)
{
	만약 (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		반환;
	}
	만약 (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { 반환 spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { 반환 spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	반환 TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	동안 (!prefix.empty()) {
		std::string_view seg = segs.peek();
		만약 (seg.empty())
			반환 false;
		size_t n = std::min(seg.size(), prefix.size());
		만약 (std::memcmp(seg.data(), prefix.data(), n) != 0)
			반환 false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	반환 true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	반환 str_size(ref) == name.size() && begins_with(ref, name);
}

static 정수 compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	동안 (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		만약 (x.empty() || y.empty())
			반환 x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		만약 (정수 diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			반환 diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	반환 find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		만약 (t == TwineNode::Key{prefix, text})
			반환 true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		반환 compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	만약 (ref == IdString::Null || ID::is_static(ref))
		반환 ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	만약 (t.is_dead())
		반환 IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		반환 compare_segments(sa, sb) == 0;
	});
	반환 stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	만약 (!t.is_suffix())
		반환 IdString::Null;
	반환 stamp(t.prefix().tag(ref.isPublic()));
}

정수 TwinePool::compare_by_name(IdString a, IdString b) const
{
	만약 (a == b)
		반환 0;
	만약 (a == IdString::Null)
		반환 -1;
	만약 (b == IdString::Null)
		반환 1;

	만약 (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		만약 (ta.is_leaf() && tb.is_leaf())
			반환 ta.text().compare(tb.text());
		만약 (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			반환 ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	반환 compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
