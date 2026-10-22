#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template struct HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	если (ready())
		вернуть;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	для (const char *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { вернуть nodes_[idx]; }
bool StaticTwines::ready() { вернуть nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
целое twine_gc_count;

std::string IdString::handle_token() const {
	вернуть stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	если (token.substr(0, 5) == "$pub@") {
		is_public = true;
		вернуть 5;
	}
	если (token.substr(0, 6) == "$priv@") {
		is_public = false;
		вернуть 6;
	}
	вернуть 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	вернуть result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	вернуть static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	если (len_ > INLINE_CAP) {
		ptr_ = new char[len_];
		memcpy(ptr_, content.data(), len_);
	} иначе {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	если (len_ > INLINE_CAP)
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
	если (this != &other)
		store(other.view());
	вернуть *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	если (this == &other)
		вернуть *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	вернуть *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	если (this == &other)
		вернуть *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	вернуть *this;
}

std::string TwineSpec::content_str() const {
	если (auto *leaf = std::get_if<Leaf>(&data))
		вернуть leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	вернуть *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	если (!name.empty() && name[0] == '\\')
		вернуть {name.substr(1), true};
	вернуть {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	для (char ch : name)
		если ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	если (this == &other)
		вернуть *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	вернуть *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	если (this == &other)
		вернуть *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	вернуть *this;
}

bool TwinePool::owns(IdString ref) const {
	вернуть ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	если (ref == IdString::Null || ID::is_static(ref))
		вернуть ref;
	вернуть ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	если constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	вернуть HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	для (auto &it : auto_prefixes)
		roots.insert(it.second);
	вернуть HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { вернуть StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	если constexpr (IdString::MAX_SERIAL == 0)
		вернуть serial;
	иначе
		вернуть serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	если (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	вернуть HashConsPool::add_inner(std::move(t));
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
	если (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	если (ref == IdString::Null)
		вернуть;
	если (ref.isPublic())
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
	вернуть out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	вернуть str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	вернуть TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	вернуть find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	если (ref != IdString::Null)
		вернуть ref;
	вернуть add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	если (name.empty())
		вернуть IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	вернуть stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	если (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		вернуть stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	если (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		вернуть stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	вернуть stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	если (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		вернуть add(auto_prefix(ap->prefix), ap->tail);
	если (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		вернуть stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	вернуть add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	если (it != auto_prefixes.end())
		вернуть it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	вернуть ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	вернуть stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	если (s.empty())
		вернуть IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	вернуть stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	если (found != IdString::Null || ref == IdString::Null)
		вернуть found;
	const TwineNode& t = src[ref.untag()];
	если (t.is_dead())
		вернуть IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	если (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	вернуть stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	вернуть "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	если (token.size() < 2 || token[0] != '#')
		вернуть IdString::Null;
	size_t sep = token.find(':');
	если (sep == std::string_view::npos)
		вернуть IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	для (целое i = 0; i < 2; i++) {
		если (parts[i].empty())
			вернуть IdString::Null;
		для (char c : parts[i]) {
			если (c < '0' || c > '9')
				вернуть IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	если (fields[0] != serial_)
		вернуть IdString::Null;
	IdString ref(fields[1]);
	если (ref == IdString::Null)
		вернуть IdString::Null;
	вернуть is_live(ref) ? ref : IdString::Null;
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
	если (ref == IdString::Null)
		вернуть;
	для (IdString cur = ref.untag(); ;) {
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
	если (ref.isPublic())
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
	пока (pos < count) {
		std::string_view seg = segs()[pos];
		если (off < seg.size())
			вернуть seg.substr(off);
		pos++;
		off = 0;
	}
	вернуть {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	для (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	вернуть total;
}

void TwineSegments::push(std::string_view seg)
{
	если (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		вернуть;
	}
	если (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { вернуть spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { вернуть spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	вернуть TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	пока (!prefix.empty()) {
		std::string_view seg = segs.peek();
		если (seg.empty())
			вернуть false;
		size_t n = std::min(seg.size(), prefix.size());
		если (std::memcmp(seg.data(), prefix.data(), n) != 0)
			вернуть false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	вернуть true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	вернуть str_size(ref) == name.size() && begins_with(ref, name);
}

static целое compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	пока (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		если (x.empty() || y.empty())
			вернуть x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		если (целое diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			вернуть diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	вернуть find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		если (t == TwineNode::Key{prefix, text})
			вернуть true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		вернуть compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	если (ref == IdString::Null || ID::is_static(ref))
		вернуть ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	если (t.is_dead())
		вернуть IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		вернуть compare_segments(sa, sb) == 0;
	});
	вернуть stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	если (!t.is_suffix())
		вернуть IdString::Null;
	вернуть stamp(t.prefix().tag(ref.isPublic()));
}

целое TwinePool::compare_by_name(IdString a, IdString b) const
{
	если (a == b)
		вернуть 0;
	если (a == IdString::Null)
		вернуть -1;
	если (b == IdString::Null)
		вернуть 1;

	если (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		если (ta.is_leaf() && tb.is_leaf())
			вернуть ta.text().compare(tb.text());
		если (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			вернуть ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	вернуть compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
