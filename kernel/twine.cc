#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template sa_nracanaa HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	ᰀᰧ (ready())
		ᰀᰧᰌ;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	for (const var_na *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { ᰀᰧᰌ nodes_[idx]; }
bool StaticTwines::ready() { ᰀᰧᰌ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
puur_naa_nka twine_gc_count;

std::string IdString::handle_token() const {
	ᰀᰧᰌ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	ᰀᰧ (token.substr(0, 5) == "$pub@") {
		is_public = true;
		ᰀᰧᰌ 5;
	}
	ᰀᰧ (token.substr(0, 6) == "$priv@") {
		is_public = false;
		ᰀᰧᰌ 6;
	}
	ᰀᰧᰌ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ᰀᰧᰌ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ᰀᰧᰌ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	ᰀᰧ (len_ > INLINE_CAP) {
		ptr_ = new var_na[len_];
		memcpy(ptr_, content.data(), len_);
	} ᰀᰧᰀ {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	ᰀᰧ (len_ > INLINE_CAP)
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
	ᰀᰧ (this != &other)
		store(other.view());
	ᰀᰧᰌ *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	ᰀᰧ (this == &other)
		ᰀᰧᰌ *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ᰀᰧᰌ *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	ᰀᰧ (this == &other)
		ᰀᰧᰌ *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ᰀᰧᰌ *this;
}

std::string TwineSpec::content_str() const {
	ᰀᰧ (auto *leaf = std::get_if<Leaf>(&data))
		ᰀᰧᰌ leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ᰀᰧᰌ *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	ᰀᰧ (!name.empty() && name[0] == '\\')
		ᰀᰧᰌ {name.substr(1), true};
	ᰀᰧᰌ {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	for (var_na ch : name)
		ᰀᰧ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	ᰀᰧ (this == &other)
		ᰀᰧᰌ *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ᰀᰧᰌ *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	ᰀᰧ (this == &other)
		ᰀᰧᰌ *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ᰀᰧᰌ *this;
}

bool TwinePool::owns(IdString ref) const {
	ᰀᰧᰌ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	ᰀᰧ (ref == IdString::Null || ID::is_static(ref))
		ᰀᰧᰌ ref;
	ᰀᰧᰌ ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	ᰀᰧ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	ᰀᰧᰌ HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	for (auto &it : auto_prefixes)
		roots.insert(it.second);
	ᰀᰧᰌ HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { ᰀᰧᰌ StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	ᰀᰧ constexpr (IdString::MAX_SERIAL == 0)
		ᰀᰧᰌ serial;
	ᰀᰧᰀ
		ᰀᰧᰌ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	ᰀᰧ (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	ᰀᰧᰌ HashConsPool::add_inner(std::move(t));
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
	ᰀᰧ (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	ᰀᰧ (ref == IdString::Null)
		ᰀᰧᰌ;
	ᰀᰧ (ref.isPublic())
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
	ᰀᰧᰌ out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	ᰀᰧᰌ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	ᰀᰧᰌ TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	ᰀᰧᰌ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	ᰀᰧ (ref != IdString::Null)
		ᰀᰧᰌ ref;
	ᰀᰧᰌ add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	ᰀᰧ (name.empty())
		ᰀᰧᰌ IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ᰀᰧᰌ stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	ᰀᰧ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ᰀᰧᰌ stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	ᰀᰧ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ᰀᰧᰌ stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ᰀᰧᰌ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	ᰀᰧ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ᰀᰧᰌ add(auto_prefix(ap->prefix), ap->tail);
	ᰀᰧ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ᰀᰧᰌ stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ᰀᰧᰌ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	ᰀᰧ (it != auto_prefixes.end())
		ᰀᰧᰌ it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ᰀᰧᰌ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ᰀᰧᰌ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	ᰀᰧ (s.empty())
		ᰀᰧᰌ IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ᰀᰧᰌ stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	ᰀᰧ (found != IdString::Null || ref == IdString::Null)
		ᰀᰧᰌ found;
	const TwineNode& t = src[ref.untag()];
	ᰀᰧ (t.is_dead())
		ᰀᰧᰌ IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	ᰀᰧ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ᰀᰧᰌ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	ᰀᰧᰌ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	ᰀᰧ (token.size() < 2 || token[0] != '#')
		ᰀᰧᰌ IdString::Null;
	size_t sep = token.find(':');
	ᰀᰧ (sep == std::string_view::npos)
		ᰀᰧᰌ IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	for (puur_naa_nka i = 0; i < 2; i++) {
		ᰀᰧ (parts[i].empty())
			ᰀᰧᰌ IdString::Null;
		for (var_na c : parts[i]) {
			ᰀᰧ (c < '0' || c > '9')
				ᰀᰧᰌ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ᰀᰧ (fields[0] != serial_)
		ᰀᰧᰌ IdString::Null;
	IdString ref(fields[1]);
	ᰀᰧ (ref == IdString::Null)
		ᰀᰧᰌ IdString::Null;
	ᰀᰧᰌ is_live(ref) ? ref : IdString::Null;
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
	ᰀᰧ (ref == IdString::Null)
		ᰀᰧᰌ;
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
	ᰀᰧ (ref.isPublic())
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
	ᰀᰬ (pos < count) {
		std::string_view seg = segs()[pos];
		ᰀᰧ (off < seg.size())
			ᰀᰧᰌ seg.substr(off);
		pos++;
		off = 0;
	}
	ᰀᰧᰌ {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	for (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	ᰀᰧᰌ total;
}

void TwineSegments::push(std::string_view seg)
{
	ᰀᰧ (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		ᰀᰧᰌ;
	}
	ᰀᰧ (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { ᰀᰧᰌ spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { ᰀᰧᰌ spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	ᰀᰧᰌ TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	ᰀᰬ (!prefix.empty()) {
		std::string_view seg = segs.peek();
		ᰀᰧ (seg.empty())
			ᰀᰧᰌ false;
		size_t n = std::min(seg.size(), prefix.size());
		ᰀᰧ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ᰀᰧᰌ false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ᰀᰧᰌ true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	ᰀᰧᰌ str_size(ref) == name.size() && begins_with(ref, name);
}

static puur_naa_nka compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ᰀᰬ (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		ᰀᰧ (x.empty() || y.empty())
			ᰀᰧᰌ x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		ᰀᰧ (puur_naa_nka diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ᰀᰧᰌ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	ᰀᰧᰌ find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		ᰀᰧ (t == TwineNode::Key{prefix, text})
			ᰀᰧᰌ true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		ᰀᰧᰌ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	ᰀᰧ (ref == IdString::Null || ID::is_static(ref))
		ᰀᰧᰌ ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	ᰀᰧ (t.is_dead())
		ᰀᰧᰌ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		ᰀᰧᰌ compare_segments(sa, sb) == 0;
	});
	ᰀᰧᰌ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	ᰀᰧ (!t.is_suffix())
		ᰀᰧᰌ IdString::Null;
	ᰀᰧᰌ stamp(t.prefix().tag(ref.isPublic()));
}

puur_naa_nka TwinePool::compare_by_name(IdString a, IdString b) const
{
	ᰀᰧ (a == b)
		ᰀᰧᰌ 0;
	ᰀᰧ (a == IdString::Null)
		ᰀᰧᰌ -1;
	ᰀᰧ (b == IdString::Null)
		ᰀᰧᰌ 1;

	ᰀᰧ (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		ᰀᰧ (ta.is_leaf() && tb.is_leaf())
			ᰀᰧᰌ ta.text().compare(tb.text());
		ᰀᰧ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ᰀᰧᰌ ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	ᰀᰧᰌ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
