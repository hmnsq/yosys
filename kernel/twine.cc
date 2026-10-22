#include "kernel/twine.h"
#include "kernel/hashcons_impl.h"
#include "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template sa_nracanaa HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

void StaticTwines::init() {
	အကယ်၍ (ready())
		ပြန်ပေး;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	အတွက် (const var_na *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

const TwineNode &StaticTwines::node(size_t idx) { ပြန်ပေး nodes_[idx]; }
bool StaticTwines::ready() { ပြန်ပေး nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
ကိန်းပြည့် twine_gc_count;

std::string IdString::handle_token() const {
	ပြန်ပေး stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	အကယ်၍ (token.substr(0, 5) == "$pub@") {
		is_public = true;
		ပြန်ပေး 5;
	}
	အကယ်၍ (token.substr(0, 6) == "$priv@") {
		is_public = false;
		ပြန်ပေး 6;
	}
	ပြန်ပေး 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ပြန်ပေး result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ပြန်ပေး static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

void SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	အကယ်၍ (len_ > INLINE_CAP) {
		ptr_ = new var_na[len_];
		memcpy(ptr_, content.data(), len_);
	} မဟုတ်ပါက {
		memcpy(inl_, content.data(), len_);
	}
}

void SmallString::release()
{
	အကယ်၍ (len_ > INLINE_CAP)
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
	အကယ်၍ (this != &other)
		store(other.view());
	ပြန်ပေး *this;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	အကယ်၍ (this == &other)
		ပြန်ပေး *this;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ပြန်ပေး *this;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	အကယ်၍ (this == &other)
		ပြန်ပေး *this;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ပြန်ပေး *this;
}

std::string TwineSpec::content_str() const {
	အကယ်၍ (auto *leaf = std::get_if<Leaf>(&data))
		ပြန်ပေး leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ပြန်ပေး *autosfx.prefix + autosfx.tail;
}

static std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	အကယ်၍ (!name.empty() && name[0] == '\\')
		ပြန်ပေး {name.substr(1), true};
	ပြန်ပေး {name, false};
}

static void check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	အတွက် (var_na ch : name)
		အကယ်၍ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(const TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(const TwinePool& other) {
	အကယ်၍ (this == &other)
		ပြန်ပေး *this;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ပြန်ပေး *this;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	အကယ်၍ (this == &other)
		ပြန်ပေး *this;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ပြန်ပေး *this;
}

bool TwinePool::owns(IdString ref) const {
	ပြန်ပေး ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) const {
	အကယ်၍ (ref == IdString::Null || ID::is_static(ref))
		ပြန်ပေး ref;
	ပြန်ပေး ref.stamped(serial_);
}

void TwinePool::check_owned(IdString ref) const {
	အကယ်၍ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

const TwineNode& TwinePool::operator[](IdString ref) const {
	check_owned(ref);
	ပြန်ပေး HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	အတွက် (auto &it : auto_prefixes)
		roots.insert(it.second);
	ပြန်ပေး HashConsPool::gc(roots);
}

const TwineNode& TwinePool::static_node(size_t idx) { ပြန်ပေး StaticTwines::node(idx); }
void TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	static size_t counter = 0;
	size_t serial = ++counter;
	အကယ်၍ constexpr (IdString::MAX_SERIAL == 0)
		ပြန်ပေး serial;
	မဟုတ်ပါက
		ပြန်ပေး serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	အကယ်၍ (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	ပြန်ပေး HashConsPool::add_inner(std::move(t));
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
	အကယ်၍ (ref.isPublic())
		os << " pub";
}

void TwinePool::append_str(IdString ref, std::string& out) const {
	အကယ်၍ (ref == IdString::Null)
		ပြန်ပေး;
	အကယ်၍ (ref.isPublic())
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
	ပြန်ပေး out;
}

std::string TwinePool::unescaped_str(IdString ref) const {
	ပြန်ပေး str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) const {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*this)[IdString(prefix)].hash();
	ပြန်ပေး TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) const {
	ပြန်ပေး find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	အကယ်၍ (ref != IdString::Null)
		ပြန်ပေး ref;
	ပြန်ပေး add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) const {
	အကယ်၍ (name.empty())
		ပြန်ပေး IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ပြန်ပေး stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) const {
	အကယ်၍ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ပြန်ပေး stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	အကယ်၍ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ပြန်ပေး stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ပြန်ပေး stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	အကယ်၍ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ပြန်ပေး add(auto_prefix(ap->prefix), ap->tail);
	အကယ်၍ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ပြန်ပေး stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	const TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ပြန်ပေး add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(const std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	အကယ်၍ (it != auto_prefixes.end())
		ပြန်ပေး it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ပြန်ပေး ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ပြန်ပေး stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	အကယ်၍ (s.empty())
		ပြန်ပေး IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ပြန်ပေး stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(const TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	အကယ်၍ (found != IdString::Null || ref == IdString::Null)
		ပြန်ပေး found;
	const TwineNode& t = src[ref.untag()];
	အကယ်၍ (t.is_dead())
		ပြန်ပေး IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	အကယ်၍ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ပြန်ပေး stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) const {
	ပြန်ပေး "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) const {
	အကယ်၍ (token.size() < 2 || token[0] != '#')
		ပြန်ပေး IdString::Null;
	size_t sep = token.find(':');
	အကယ်၍ (sep == std::string_view::npos)
		ပြန်ပေး IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	အတွက် (ကိန်းပြည့် i = 0; i < 2; i++) {
		အကယ်၍ (parts[i].empty())
			ပြန်ပေး IdString::Null;
		အတွက် (var_na c : parts[i]) {
			အကယ်၍ (c < '0' || c > '9')
				ပြန်ပေး IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	အကယ်၍ (fields[0] != serial_)
		ပြန်ပေး IdString::Null;
	IdString ref(fields[1]);
	အကယ်၍ (ref == IdString::Null)
		ပြန်ပေး IdString::Null;
	ပြန်ပေး is_live(ref) ? ref : IdString::Null;
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
	အကယ်၍ (ref == IdString::Null)
		ပြန်ပေး;
	အတွက် (IdString cur = ref.untag(); ;) {
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
	အကယ်၍ (ref.isPublic())
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
	နေစဉ် (pos < count) {
		std::string_view seg = segs()[pos];
		အကယ်၍ (off < seg.size())
			ပြန်ပေး seg.substr(off);
		pos++;
		off = 0;
	}
	ပြန်ပေး {};
}

void TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() const
{
	size_t total = 0;
	အတွက် (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	ပြန်ပေး total;
}

void TwineSegments::push(std::string_view seg)
{
	အကယ်၍ (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		ပြန်ပေး;
	}
	အကယ်၍ (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { ပြန်ပေး spill.empty() ? inline_segs : spill.data(); }
const std::string_view *TwineSegments::segs() const { ပြန်ပေး spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) const
{
	ပြန်ပေး TwineSegments(*this, ref).total_size();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) const
{
	TwineSegments segs(*this, ref);
	နေစဉ် (!prefix.empty()) {
		std::string_view seg = segs.peek();
		အကယ်၍ (seg.empty())
			ပြန်ပေး false;
		size_t n = std::min(seg.size(), prefix.size());
		အကယ်၍ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ပြန်ပေး false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ပြန်ပေး true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) const
{
	ပြန်ပေး str_size(ref) == name.size() && begins_with(ref, name);
}

static ကိန်းပြည့် compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	နေစဉ် (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		အကယ်၍ (x.empty() || y.empty())
			ပြန်ပေး x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		အကယ်၍ (ကိန်းပြည့် diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ပြန်ပေး diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) const {
	ပြန်ပေး find_hashed(hash, [&](IdString ref) {
		const TwineNode &t = (*this)[ref];
		အကယ်၍ (t == TwineNode::Key{prefix, text})
			ပြန်ပေး true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*this, ref), sb(*this, head, text);
		ပြန်ပေး compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(const TwinePool& src, IdString ref) const {
	အကယ်၍ (ref == IdString::Null || ID::is_static(ref))
		ပြန်ပေး ref;
	IdString idx = ref.untag();
	const TwineNode& t = src[idx];
	အကယ်၍ (t.is_dead())
		ပြန်ပေး IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*this, cand), sb(src, idx);
		ပြန်ပေး compare_segments(sa, sb) == 0;
	});
	ပြန်ပေး stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) const {
	const TwineNode &t = (*this)[ref];
	အကယ်၍ (!t.is_suffix())
		ပြန်ပေး IdString::Null;
	ပြန်ပေး stamp(t.prefix().tag(ref.isPublic()));
}

ကိန်းပြည့် TwinePool::compare_by_name(IdString a, IdString b) const
{
	အကယ်၍ (a == b)
		ပြန်ပေး 0;
	အကယ်၍ (a == IdString::Null)
		ပြန်ပေး -1;
	အကယ်၍ (b == IdString::Null)
		ပြန်ပေး 1;

	အကယ်၍ (a.isPublic() == b.isPublic()) {
		const TwineNode &ta = (*this)[a.untag()];
		const TwineNode &tb = (*this)[b.untag()];
		အကယ်၍ (ta.is_leaf() && tb.is_leaf())
			ပြန်ပေး ta.text().compare(tb.text());
		အကယ်၍ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ပြန်ပေး ta.text().compare(tb.text());
	}

	TwineSegments sa(*this, a), sb(*this, b);
	ပြန်ပေး compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
