#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template sa_nracanaa HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	ᨀᨘ (ready())
		ᨀᨘᨒ;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	for (const var_na *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { ᨀᨘᨒ nodes_[idx]; }
bool StaticTwines::ready() { ᨀᨘᨒ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
puur_naa_nka twine_gc_count;

std::string IdString::handle_token() const {
	ᨀᨘᨒ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	ᨀᨘ (token.substr(0, 5) == "$pub@") {
		is_public = true;
		ᨀᨘᨒ 5;
	}
	ᨀᨘ (token.substr(0, 6) == "$priv@") {
		is_public = false;
		ᨀᨘᨒ 6;
	}
	ᨀᨘᨒ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ᨀᨘᨒ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ᨀᨘᨒ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	ᨀᨘ (len_ > INLINE_CAP) {
		ptr_ = new var_na[len_];
		memcpy(ptr_, content.data(), len_);
	} ᨀᨘᨁ {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	ᨀᨘ (len_ > INLINE_CAP)
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
	ᨀᨘ (this != &other)
		store(other.view());
	ᨀᨘᨒ *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	ᨀᨘ (this == &other)
		ᨀᨘᨒ *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ᨀᨘᨒ *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	ᨀᨘ (this == &other)
		ᨀᨘᨒ *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ᨀᨘᨒ *this;
}

std::string TwineSpec::content_str() const {
	ᨀᨘ (auto *leaf = std::get_if<Leaf>(&data))
		ᨀᨘᨒ leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ᨀᨘᨒ *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	ᨀᨘ (!name.empty() && name[0] == '\\')
		ᨀᨘᨒ {name.substr(1), true};
	ᨀᨘᨒ {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	for (var_na ch : name)
		ᨀᨘ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	ᨀᨘ (this == &other)
		ᨀᨘᨒ *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ᨀᨘᨒ *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	ᨀᨘ (this == &other)
		ᨀᨘᨒ *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ᨀᨘᨒ *this;
}

bool TwinePool::owns(IdString ref) const {
	ᨀᨘᨒ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	ᨀᨘ (ref == IdString::Null || ID::is_static(ref))
		ᨀᨘᨒ ref;
	ᨀᨘᨒ ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	ᨀᨘ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	ᨀᨘᨒ HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	for (auto &it : auto_prefixes)
		roots.insert(it.second);
	ᨀᨘᨒ HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { ᨀᨘᨒ StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	ᨀᨘ constexpr (IdString::MAX_SERIAL == 0)
		ᨀᨘᨒ serial;
	ᨀᨘᨁ
		ᨀᨘᨒ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	ᨀᨘ (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	ᨀᨘᨒ HashConsPool::add_inner(std::move(t));
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
	ᨀᨘ (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	ᨀᨘ (ref == IdString::Null)
		ᨀᨘᨒ;
	ᨀᨘ (ref.isPublic())
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
	ᨀᨘᨒ out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	ᨀᨘᨒ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	ᨀᨘᨒ TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	ᨀᨘᨒ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	ᨀᨘ (ref != IdString::Null)
		ᨀᨘᨒ ref;
	ᨀᨘᨒ add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	ᨀᨘ (name.empty())
		ᨀᨘᨒ IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ᨀᨘᨒ stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	ᨀᨘ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ᨀᨘᨒ stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	ᨀᨘ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ᨀᨘᨒ stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ᨀᨘᨒ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	ᨀᨘ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ᨀᨘᨒ add(auto_prefix(ap->prefix), ap->tail);
	ᨀᨘ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ᨀᨘᨒ stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ᨀᨘᨒ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	ᨀᨘ (it != auto_prefixes.end())
		ᨀᨘᨒ it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ᨀᨘᨒ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ᨀᨘᨒ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	ᨀᨘ (s.empty())
		ᨀᨘᨒ IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ᨀᨘᨒ stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	ᨀᨘ (found != IdString::Null || ref == IdString::Null)
		ᨀᨘᨒ found;
	const TwineNode& t = src[ref.untag()];
	ᨀᨘ (t.is_dead())
		ᨀᨘᨒ IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	ᨀᨘ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ᨀᨘᨒ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	ᨀᨘᨒ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	ᨀᨘ (token.size() < 2 || token[0] != '#')
		ᨀᨘᨒ IdString::Null;
	size_t sep = token.find(':');
	ᨀᨘ (sep == std::string_view::npos)
		ᨀᨘᨒ IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	for (puur_naa_nka i = 0; i < 2; i++) {
		ᨀᨘ (parts[i].empty())
			ᨀᨘᨒ IdString::Null;
		for (var_na c : parts[i]) {
			ᨀᨘ (c < '0' || c > '9')
				ᨀᨘᨒ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ᨀᨘ (fields[0] != serial_)
		ᨀᨘᨒ IdString::Null;
	IdString ref(fields[1]);
	ᨀᨘ (ref == IdString::Null)
		ᨀᨘᨒ IdString::Null;
	ᨀᨘᨒ is_live(ref) ? ref : IdString::Null;
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
	ᨀᨘ (ref == IdString::Null)
		ᨀᨘᨒ;
	for (IdString cur = ref.untag(); ;) {
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
	ᨀᨘ (ref.isPublic())
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
	ᨀᨛ (pos < count) {
		std::string_view seg = segs()[pos];
		ᨀᨘ (off < seg.size())
			ᨀᨘᨒ seg.substr(off);
		pos++;
		off = 0;
	}
	ᨀᨘᨒ {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	for (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	ᨀᨘᨒ total;
}

void TwineSegments::push(std::string_view seg)
{
	ᨀᨘ (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		ᨀᨘᨒ;
	}
	ᨀᨘ (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { ᨀᨘᨒ spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { ᨀᨘᨒ spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	ᨀᨘᨒ TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	ᨀᨛ (!prefix.empty()) {
		std::string_view seg = segs.peek();
		ᨀᨘ (seg.empty())
			ᨀᨘᨒ false;
		size_t n = std::min(seg.size(), prefix.size());
		ᨀᨘ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ᨀᨘᨒ false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ᨀᨘᨒ true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	ᨀᨘᨒ str_size(ref) == name.size() && begins_with(ref, name);
}

static puur_naa_nka compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ᨀᨛ (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		ᨀᨘ (x.empty() || y.empty())
			ᨀᨘᨒ x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		ᨀᨘ (puur_naa_nka diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ᨀᨘᨒ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	ᨀᨘᨒ find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		ᨀᨘ (t == TwineNode::Key{prefix, text})
			ᨀᨘᨒ true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		ᨀᨘᨒ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	ᨀᨘ (ref == IdString::Null || ID::is_static(ref))
		ᨀᨘᨒ ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	ᨀᨘ (t.is_dead())
		ᨀᨘᨒ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		ᨀᨘᨒ compare_segments(sa, sb) == 0;
	});
	ᨀᨘᨒ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	ᨀᨘ (!t.is_suffix())
		ᨀᨘᨒ IdString::Null;
	ᨀᨘᨒ stamp(t.prefix().tag(ref.isPublic()));
}

puur_naa_nka TwinePool::compare_by_name(IdString a, IdString b) const
{
	ᨀᨘ (a == b)
		ᨀᨘᨒ 0;
	ᨀᨘ (a == IdString::Null)
		ᨀᨘᨒ -1;
	ᨀᨘ (b == IdString::Null)
		ᨀᨘᨒ 1;

	ᨀᨘ (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		ᨀᨘ (ta.is_leaf() && tb.is_leaf())
			ᨀᨘᨒ ta.text().compare(tb.text());
		ᨀᨘ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ᨀᨘᨒ ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	ᨀᨘᨒ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
