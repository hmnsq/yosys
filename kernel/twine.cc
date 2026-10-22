#அந்தர்பாவ "kernel/twine.h"
#அந்தர்பாவ "kernel/hashcons_impl.h"
#அந்தர்பாவ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template கட்டமைப்பு HashConsPool<TwinePool, Twine_முனை, IdString>;

std::vector<Twine_முனை> StaticTwines::nodes_;

வெறுமை StaticTwines::init() {
	எனில் (ready())
		திரும்பு;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	க்கு (மாறிலி எழுத்து *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

மாறிலி Twine_முனை &StaticTwines::முனை(size_t idx) { திரும்பு nodes_[idx]; }
தருக்க StaticTwines::ready() { திரும்பு nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
முழு twine_gc_மொத்தம்;

std::string IdString::handle_token() மாறிலி {
	திரும்பு stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, தருக்க &is_public) {
	எனில் (token.substr(0, 5) == "$pub@") {
		is_public = உண்மை;
		திரும்பு 5;
	}
	எனில் (token.substr(0, 6) == "$priv@") {
		is_public = பொய்;
		திரும்பு 6;
	}
	திரும்பு 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	திரும்பு result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	திரும்பு static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

வெறுமை SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	எனில் (len_ > INLINE_CAP) {
		ptr_ = புதிய எழுத்து[len_];
		memcpy(ptr_, content.data(), len_);
	} இல்லையெனில் {
		memcpy(inl_, content.data(), len_);
	}
}

வெறுமை SmallString::release()
{
	எனில் (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(மாறிலி SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(மாறிலி SmallString &other)
{
	எனில் (இது != &other)
		store(other.view());
	திரும்பு *இது;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	எனில் (இது == &other)
		திரும்பு *இது;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	திரும்பு *இது;
}

SmallString::~SmallString() { release(); }

Twine_முனை::Twine_முனை(Twine_முனை &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_முனை &Twine_முனை::operator=(Twine_முனை &&other) noexcept
{
	எனில் (இது == &other)
		திரும்பு *இது;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	திரும்பு *இது;
}

std::string TwineSpec::content_str() மாறிலி {
	எனில் (auto *leaf = std::get_if<Leaf>(&data))
		திரும்பு leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	திரும்பு *autosfx.prefix + autosfx.tail;
}

நிலையான std::pair<std::string_view, தருக்க> unescape_name(std::string_view name) {
	எனில் (!name.empty() && name[0] == '\\')
		திரும்பு {name.substr(1), உண்மை};
	திரும்பு {name, பொய்};
}

நிலையான வெறுமை check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	க்கு (எழுத்து ch : name)
		எனில் ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(மாறிலி TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(மாறிலி TwinePool& other) {
	எனில் (இது == &other)
		திரும்பு *இது;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	திரும்பு *இது;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	எனில் (இது == &other)
		திரும்பு *இது;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	திரும்பு *இது;
}

தருக்க TwinePool::owns(IdString ref) மாறிலி {
	திரும்பு ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) மாறிலி {
	எனில் (ref == IdString::Null || ID::is_static(ref))
		திரும்பு ref;
	திரும்பு ref.stamped(serial_);
}

வெறுமை TwinePool::check_owned(IdString ref) மாறிலி {
	எனில் constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

மாறிலி Twine_முனை& TwinePool::operator[](IdString ref) மாறிலி {
	check_owned(ref);
	திரும்பு HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	க்கு (auto &it : auto_prefixes)
		roots.செருகு(it.second);
	திரும்பு HashConsPool::gc(roots);
}

மாறிலி Twine_முனை& TwinePool::static_முனை(size_t idx) { திரும்பு StaticTwines::முனை(idx); }
வெறுமை TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	நிலையான size_t counter = 0;
	size_t serial = ++counter;
	எனில் constexpr (IdString::MAX_SERIAL == 0)
		திரும்பு serial;
	இல்லையெனில்
		திரும்பு serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_முனை t) {
	எனில் (free_list.empty() && STATIC_மொத்தம் + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_மொத்தம்);
	திரும்பு HashConsPool::add_inner(std::move(t));
}

வெறுமை TwinePool::dump(IdString ref, std::ostream& os) மாறிலி {
	மாறிலி Twine_முனை& twine = (*இது)[ref];
	தேர்வு (twine.kind()) {
	நிலை Twine_முனை::Kind::Dead:
		os << "Dead()";
		நிறுத்து;
	நிலை Twine_முனை::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		நிறுத்து;
	நிலை Twine_முனை::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		நிறுத்து;
	}
	எனில் (ref.isPublic())
		os << " pub";
}

வெறுமை TwinePool::append_str(IdString ref, std::string& out) மாறிலி {
	எனில் (ref == IdString::Null)
		திரும்பு;
	எனில் (ref.isPublic())
		out += '\\';
	மாறிலி Twine_முனை& twine = (*இது)[ref];
	தேர்வு (twine.kind()) {
	நிலை Twine_முனை::Kind::Dead:
		நிறுத்து;
	நிலை Twine_முனை::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	நிலை Twine_முனை::Kind::Leaf:
		out += twine.text();
		நிறுத்து;
	}
}

std::string TwinePool::str(IdString ref) மாறிலி {
	std::string out;
	append_str(ref, out);
	திரும்பு out;
}

std::string TwinePool::unescaped_str(IdString ref) மாறிலி {
	திரும்பு str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) மாறிலி {
	uint32_t hash = prefix == Twine_முனை::NO_PREFIX ? 0 : (*இது)[IdString(prefix)].hash();
	திரும்பு Twine_முனை::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) மாறிலி {
	திரும்பு find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	எனில் (ref != IdString::Null)
		திரும்பு ref;
	திரும்பு add_inner(Twine_முனை{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) மாறிலி {
	எனில் (name.empty())
		திரும்பு IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	திரும்பு stamp(find_content(Twine_முனை::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) மாறிலி {
	எனில் (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		திரும்பு stamp(find_content(Twine_முனை::NO_PREFIX, t.content_str()));
	எனில் (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		திரும்பு stamp(find_content(Twine_முனை::NO_PREFIX, leaf->s));
	மாறிலி TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	திரும்பு stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	எனில் (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		திரும்பு add(auto_prefix(ap->prefix), ap->tail);
	எனில் (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		திரும்பு stamp(intern(Twine_முனை::NO_PREFIX, leaf->s));
	மாறிலி TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	திரும்பு add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(மாறிலி std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	எனில் (it != auto_prefixes.end())
		திரும்பு it->second;
	IdString ref = intern(Twine_முனை::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	திரும்பு ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	திரும்பு stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	எனில் (s.empty())
		திரும்பு IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	திரும்பு stamp(intern(Twine_முனை::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(மாறிலி TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	எனில் (found != IdString::Null || ref == IdString::Null)
		திரும்பு found;
	மாறிலி Twine_முனை& t = src[ref.untag()];
	எனில் (t.is_dead())
		திரும்பு IdString::Null;
	uint32_t prefix = Twine_முனை::NO_PREFIX;
	எனில் (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	திரும்பு stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) மாறிலி {
	திரும்பு "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) மாறிலி {
	எனில் (token.size() < 2 || token[0] != '#')
		திரும்பு IdString::Null;
	size_t sep = token.find(':');
	எனில் (sep == std::string_view::npos)
		திரும்பு IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	க்கு (முழு i = 0; i < 2; i++) {
		எனில் (parts[i].empty())
			திரும்பு IdString::Null;
		க்கு (எழுத்து c : parts[i]) {
			எனில் (c < '0' || c > '9')
				திரும்பு IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	எனில் (fields[0] != serial_)
		திரும்பு IdString::Null;
	IdString ref(fields[1]);
	எனில் (ref == IdString::Null)
		திரும்பு IdString::Null;
	திரும்பு is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
கட்டமைப்பு TwineSegments {
	நிலையான constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(மாறிலி TwinePool &pool, IdString ref);
	TwineSegments(மாறிலி TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	வெறுமை advance(size_t n);
	size_t total_size() மாறிலி;
தனிப்பட்ட:
	வெறுமை தள்ளு(std::string_view seg);
	std::string_view *segs();
	மாறிலி std::string_view *segs() மாறிலி;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t மொத்தம் = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(மாறிலி TwinePool &pool, IdString ref)
{
	எனில் (ref == IdString::Null)
		திரும்பு;
	க்கு (IdString cur = ref.untag(); ;) {
		மாறிலி Twine_முனை &t = pool[cur];
		தேர்வு (t.kind()) {
		நிலை Twine_முனை::Kind::Suffix:
			தள்ளு(t.text());
			cur = t.prefix();
			தொடர்;
		நிலை Twine_முனை::Kind::Leaf:
			தள்ளு(t.text());
			நிறுத்து;
		நிலை Twine_முனை::Kind::Dead:
			நிறுத்து;
		}
		நிறுத்து;
	}
	எனில் (ref.isPublic())
		தள்ளு("\\");
	std::reverse(segs(), segs() + மொத்தம்);
}

TwineSegments::TwineSegments(மாறிலி TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	தள்ளு(tail);
}

std::string_view TwineSegments::peek()
{
	போது (pos < மொத்தம்) {
		std::string_view seg = segs()[pos];
		எனில் (off < seg.size())
			திரும்பு seg.substr(off);
		pos++;
		off = 0;
	}
	திரும்பு {};
}

வெறுமை TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() மாறிலி
{
	size_t total = 0;
	க்கு (size_t i = 0; i < மொத்தம்; i++)
		total += segs()[i].size();
	திரும்பு total;
}

வெறுமை TwineSegments::தள்ளு(std::string_view seg)
{
	எனில் (spill.empty() && மொத்தம் < SEGMENT_INLINE_DEPTH) {
		inline_segs[மொத்தம்++] = seg;
		திரும்பு;
	}
	எனில் (spill.empty())
		spill.assign(inline_segs, inline_segs + மொத்தம்);
	spill.தள்ளு_back(seg);
	மொத்தம்++;
}

std::string_view *TwineSegments::segs() { திரும்பு spill.empty() ? inline_segs : spill.data(); }
மாறிலி std::string_view *TwineSegments::segs() மாறிலி { திரும்பு spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) மாறிலி
{
	திரும்பு TwineSegments(*இது, ref).total_size();
}

தருக்க TwinePool::begins_with(IdString ref, std::string_view prefix) மாறிலி
{
	TwineSegments segs(*இது, ref);
	போது (!prefix.empty()) {
		std::string_view seg = segs.peek();
		எனில் (seg.empty())
			திரும்பு பொய்;
		size_t n = std::min(seg.size(), prefix.size());
		எனில் (std::memcmp(seg.data(), prefix.data(), n) != 0)
			திரும்பு பொய்;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	திரும்பு உண்மை;
}

தருக்க TwinePool::name_equal(IdString ref, std::string_view name) மாறிலி
{
	திரும்பு str_size(ref) == name.size() && begins_with(ref, name);
}

நிலையான முழு compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	போது (உண்மை) {
		std::string_view x = sa.peek(), y = sb.peek();
		எனில் (x.empty() || y.empty())
			திரும்பு x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		எனில் (முழு diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			திரும்பு diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) மாறிலி {
	திரும்பு find_hashed(hash, [&](IdString ref) {
		மாறிலி Twine_முனை &t = (*இது)[ref];
		எனில் (t == Twine_முனை::சாவி_2{prefix, text})
			திரும்பு உண்மை;
		IdString head = prefix == Twine_முனை::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*இது, ref), sb(*இது, head, text);
		திரும்பு compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(மாறிலி TwinePool& src, IdString ref) மாறிலி {
	எனில் (ref == IdString::Null || ID::is_static(ref))
		திரும்பு ref;
	IdString idx = ref.untag();
	மாறிலி Twine_முனை& t = src[idx];
	எனில் (t.is_dead())
		திரும்பு IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*இது, cand), sb(src, idx);
		திரும்பு compare_segments(sa, sb) == 0;
	});
	திரும்பு stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) மாறிலி {
	மாறிலி Twine_முனை &t = (*இது)[ref];
	எனில் (!t.is_suffix())
		திரும்பு IdString::Null;
	திரும்பு stamp(t.prefix().tag(ref.isPublic()));
}

முழு TwinePool::compare_by_name(IdString a, IdString b) மாறிலி
{
	எனில் (a == b)
		திரும்பு 0;
	எனில் (a == IdString::Null)
		திரும்பு -1;
	எனில் (b == IdString::Null)
		திரும்பு 1;

	எனில் (a.isPublic() == b.isPublic()) {
		மாறிலி Twine_முனை &ta = (*இது)[a.untag()];
		மாறிலி Twine_முனை &tb = (*இது)[b.untag()];
		எனில் (ta.is_leaf() && tb.is_leaf())
			திரும்பு ta.text().compare(tb.text());
		எனில் (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			திரும்பு ta.text().compare(tb.text());
	}

	TwineSegments sa(*இது, a), sb(*இது, b);
	திரும்பு compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
