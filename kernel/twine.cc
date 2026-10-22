#ଅନ୍ତର୍ଭାଵ "kernel/twine.h"
#ଅନ୍ତର୍ଭାଵ "kernel/hashcons_impl.h"
#ଅନ୍ତର୍ଭାଵ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template ସଂରଚନା HashConsPool<TwinePool, Twine_ଗଣ୍ଠି, IdString>;

std::vector<Twine_ଗଣ୍ଠି> StaticTwines::nodes_;

ଶୂନ୍ୟ StaticTwines::init() {
	ଯଦି (ready())
		ଫେରନ୍ତୁ;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	ପାଇଁ (ଅଚଳ ଅକ୍ଷର *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ଅଚଳ Twine_ଗଣ୍ଠି &StaticTwines::ଗଣ୍ଠି(size_t idx) { ଫେରନ୍ତୁ nodes_[idx]; }
ତାର୍କିକ StaticTwines::ready() { ଫେରନ୍ତୁ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା twine_gc_ଗଣନା;

std::string IdString::handle_token() ଅଚଳ {
	ଫେରନ୍ତୁ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, ତାର୍କିକ &is_public) {
	ଯଦି (token.substr(0, 5) == "$pub@") {
		is_public = ସତ୍ୟ;
		ଫେରନ୍ତୁ 5;
	}
	ଯଦି (token.substr(0, 6) == "$priv@") {
		is_public = ମିଥ୍ୟା;
		ଫେରନ୍ତୁ 6;
	}
	ଫେରନ୍ତୁ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ଫେରନ୍ତୁ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ଫେରନ୍ତୁ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

ଶୂନ୍ୟ SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	ଯଦି (len_ > INLINE_CAP) {
		ptr_ = ନୂତନ ଅକ୍ଷର[len_];
		memcpy(ptr_, content.data(), len_);
	} ଅନ୍ୟଥା {
		memcpy(inl_, content.data(), len_);
	}
}

ଶୂନ୍ୟ SmallString::release()
{
	ଯଦି (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ଅଚଳ SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ଅଚଳ SmallString &other)
{
	ଯଦି (ଏହି != &other)
		store(other.view());
	ଫେରନ୍ତୁ *ଏହି;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	ଯଦି (ଏହି == &other)
		ଫେରନ୍ତୁ *ଏହି;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ଫେରନ୍ତୁ *ଏହି;
}

SmallString::~SmallString() { release(); }

Twine_ଗଣ୍ଠି::Twine_ଗଣ୍ଠି(Twine_ଗଣ୍ଠି &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_ଗଣ୍ଠି &Twine_ଗଣ୍ଠି::operator=(Twine_ଗଣ୍ଠି &&other) noexcept
{
	ଯଦି (ଏହି == &other)
		ଫେରନ୍ତୁ *ଏହି;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ଫେରନ୍ତୁ *ଏହି;
}

std::string TwineSpec::content_str() ଅଚଳ {
	ଯଦି (auto *leaf = std::get_if<Leaf>(&data))
		ଫେରନ୍ତୁ leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ଫେରନ୍ତୁ *autosfx.prefix + autosfx.tail;
}

ସ୍ଥିର std::pair<std::string_view, ତାର୍କିକ> unescape_name(std::string_view name) {
	ଯଦି (!name.empty() && name[0] == '\\')
		ଫେରନ୍ତୁ {name.substr(1), ସତ୍ୟ};
	ଫେରନ୍ତୁ {name, ମିଥ୍ୟା};
}

ସ୍ଥିର ଶୂନ୍ୟ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	ପାଇଁ (ଅକ୍ଷର ch : name)
		ଯଦି ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ଅଚଳ TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ଅଚଳ TwinePool& other) {
	ଯଦି (ଏହି == &other)
		ଫେରନ୍ତୁ *ଏହି;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ଫେରନ୍ତୁ *ଏହି;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	ଯଦି (ଏହି == &other)
		ଫେରନ୍ତୁ *ଏହି;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ଫେରନ୍ତୁ *ଏହି;
}

ତାର୍କିକ TwinePool::owns(IdString ref) ଅଚଳ {
	ଫେରନ୍ତୁ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ଅଚଳ {
	ଯଦି (ref == IdString::Null || ID::is_static(ref))
		ଫେରନ୍ତୁ ref;
	ଫେରନ୍ତୁ ref.stamped(serial_);
}

ଶୂନ୍ୟ TwinePool::check_owned(IdString ref) ଅଚଳ {
	ଯଦି constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ଅଚଳ Twine_ଗଣ୍ଠି& TwinePool::operator[](IdString ref) ଅଚଳ {
	check_owned(ref);
	ଫେରନ୍ତୁ HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	ପାଇଁ (auto &it : auto_prefixes)
		roots.ଭର୍ତ୍ତି(it.second);
	ଫେରନ୍ତୁ HashConsPool::gc(roots);
}

ଅଚଳ Twine_ଗଣ୍ଠି& TwinePool::static_ଗଣ୍ଠି(size_t idx) { ଫେରନ୍ତୁ StaticTwines::ଗଣ୍ଠି(idx); }
ଶୂନ୍ୟ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	ସ୍ଥିର size_t counter = 0;
	size_t serial = ++counter;
	ଯଦି constexpr (IdString::MAX_SERIAL == 0)
		ଫେରନ୍ତୁ serial;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_ଗଣ୍ଠି t) {
	ଯଦି (free_list.empty() && STATIC_ଗଣନା + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_ଗଣନା);
	ଫେରନ୍ତୁ HashConsPool::add_inner(std::move(t));
}

ଶୂନ୍ୟ TwinePool::dump(IdString ref, std::ostream& os) ଅଚଳ {
	ଅଚଳ Twine_ଗଣ୍ଠି& twine = (*ଏହି)[ref];
	ବାଛ (twine.kind()) {
	ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Dead:
		os << "Dead()";
		ଭାଙ୍ଗ;
	ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		ଭାଙ୍ଗ;
	ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		ଭାଙ୍ଗ;
	}
	ଯଦି (ref.isPublic())
		os << " pub";
}

ଶୂନ୍ୟ TwinePool::append_str(IdString ref, std::string& out) ଅଚଳ {
	ଯଦି (ref == IdString::Null)
		ଫେରନ୍ତୁ;
	ଯଦି (ref.isPublic())
		out += '\\';
	ଅଚଳ Twine_ଗଣ୍ଠି& twine = (*ଏହି)[ref];
	ବାଛ (twine.kind()) {
	ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Dead:
		ଭାଙ୍ଗ;
	ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Leaf:
		out += twine.text();
		ଭାଙ୍ଗ;
	}
}

std::string TwinePool::str(IdString ref) ଅଚଳ {
	std::string out;
	append_str(ref, out);
	ଫେରନ୍ତୁ out;
}

std::string TwinePool::unescaped_str(IdString ref) ଅଚଳ {
	ଫେରନ୍ତୁ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ଅଚଳ {
	uint32_t hash = prefix == Twine_ଗଣ୍ଠି::NO_PREFIX ? 0 : (*ଏହି)[IdString(prefix)].hash();
	ଫେରନ୍ତୁ Twine_ଗଣ୍ଠି::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ଅଚଳ {
	ଫେରନ୍ତୁ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	ଯଦି (ref != IdString::Null)
		ଫେରନ୍ତୁ ref;
	ଫେରନ୍ତୁ add_inner(Twine_ଗଣ୍ଠି{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ଅଚଳ {
	ଯଦି (name.empty())
		ଫେରନ୍ତୁ IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ଫେରନ୍ତୁ stamp(find_content(Twine_ଗଣ୍ଠି::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ଅଚଳ {
	ଯଦି (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ଫେରନ୍ତୁ stamp(find_content(Twine_ଗଣ୍ଠି::NO_PREFIX, t.content_str()));
	ଯଦି (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ଫେରନ୍ତୁ stamp(find_content(Twine_ଗଣ୍ଠି::NO_PREFIX, leaf->s));
	ଅଚଳ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ଫେରନ୍ତୁ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	ଯଦି (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ଫେରନ୍ତୁ add(auto_prefix(ap->prefix), ap->tail);
	ଯଦି (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ଫେରନ୍ତୁ stamp(intern(Twine_ଗଣ୍ଠି::NO_PREFIX, leaf->s));
	ଅଚଳ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ଫେରନ୍ତୁ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ଅଚଳ std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	ଯଦି (it != auto_prefixes.end())
		ଫେରନ୍ତୁ it->second;
	IdString ref = intern(Twine_ଗଣ୍ଠି::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ଫେରନ୍ତୁ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ଫେରନ୍ତୁ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	ଯଦି (s.empty())
		ଫେରନ୍ତୁ IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ଫେରନ୍ତୁ stamp(intern(Twine_ଗଣ୍ଠି::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ଅଚଳ TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	ଯଦି (found != IdString::Null || ref == IdString::Null)
		ଫେରନ୍ତୁ found;
	ଅଚଳ Twine_ଗଣ୍ଠି& t = src[ref.untag()];
	ଯଦି (t.is_dead())
		ଫେରନ୍ତୁ IdString::Null;
	uint32_t prefix = Twine_ଗଣ୍ଠି::NO_PREFIX;
	ଯଦି (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ଫେରନ୍ତୁ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ଅଚଳ {
	ଫେରନ୍ତୁ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ଅଚଳ {
	ଯଦି (token.size() < 2 || token[0] != '#')
		ଫେରନ୍ତୁ IdString::Null;
	size_t sep = token.find(':');
	ଯଦି (sep == std::string_view::npos)
		ଫେରନ୍ତୁ IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 2; i++) {
		ଯଦି (parts[i].empty())
			ଫେରନ୍ତୁ IdString::Null;
		ପାଇଁ (ଅକ୍ଷର c : parts[i]) {
			ଯଦି (c < '0' || c > '9')
				ଫେରନ୍ତୁ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ଯଦି (fields[0] != serial_)
		ଫେରନ୍ତୁ IdString::Null;
	IdString ref(fields[1]);
	ଯଦି (ref == IdString::Null)
		ଫେରନ୍ତୁ IdString::Null;
	ଫେରନ୍ତୁ is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ସଂରଚନା TwineSegments {
	ସ୍ଥିର constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ଅଚଳ TwinePool &pool, IdString ref);
	TwineSegments(ଅଚଳ TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	ଶୂନ୍ୟ advance(size_t n);
	size_t total_size() ଅଚଳ;
ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ ଠେଲ(std::string_view seg);
	std::string_view *segs();
	ଅଚଳ std::string_view *segs() ଅଚଳ;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t ଗଣନା_2 = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ଅଚଳ TwinePool &pool, IdString ref)
{
	ଯଦି (ref == IdString::Null)
		ଫେରନ୍ତୁ;
	ପାଇଁ (IdString cur = ref.untag(); ;) {
		ଅଚଳ Twine_ଗଣ୍ଠି &t = pool[cur];
		ବାଛ (t.kind()) {
		ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Suffix:
			ଠେଲ(t.text());
			cur = t.prefix();
			ଜାରି;
		ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Leaf:
			ଠେଲ(t.text());
			ଭାଙ୍ଗ;
		ସ୍ଥିତି Twine_ଗଣ୍ଠି::Kind::Dead:
			ଭାଙ୍ଗ;
		}
		ଭାଙ୍ଗ;
	}
	ଯଦି (ref.isPublic())
		ଠେଲ("\\");
	std::reverse(segs(), segs() + ଗଣନା_2);
}

TwineSegments::TwineSegments(ଅଚଳ TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	ଠେଲ(tail);
}

std::string_view TwineSegments::peek()
{
	ଯେତେବେଳେ (pos < ଗଣନା_2) {
		std::string_view seg = segs()[pos];
		ଯଦି (off < seg.size())
			ଫେରନ୍ତୁ seg.substr(off);
		pos++;
		off = 0;
	}
	ଫେରନ୍ତୁ {};
}

ଶୂନ୍ୟ TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ଅଚଳ
{
	size_t total = 0;
	ପାଇଁ (size_t i = 0; i < ଗଣନା_2; i++)
		total += segs()[i].size();
	ଫେରନ୍ତୁ total;
}

ଶୂନ୍ୟ TwineSegments::ଠେଲ(std::string_view seg)
{
	ଯଦି (spill.empty() && ଗଣନା_2 < SEGMENT_INLINE_DEPTH) {
		inline_segs[ଗଣନା_2++] = seg;
		ଫେରନ୍ତୁ;
	}
	ଯଦି (spill.empty())
		spill.assign(inline_segs, inline_segs + ଗଣନା_2);
	spill.ଠେଲ_back(seg);
	ଗଣନା_2++;
}

std::string_view *TwineSegments::segs() { ଫେରନ୍ତୁ spill.empty() ? inline_segs : spill.data(); }
ଅଚଳ std::string_view *TwineSegments::segs() ଅଚଳ { ଫେରନ୍ତୁ spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ଅଚଳ
{
	ଫେରନ୍ତୁ TwineSegments(*ଏହି, ref).total_size();
}

ତାର୍କିକ TwinePool::begins_with(IdString ref, std::string_view prefix) ଅଚଳ
{
	TwineSegments segs(*ଏହି, ref);
	ଯେତେବେଳେ (!prefix.empty()) {
		std::string_view seg = segs.peek();
		ଯଦି (seg.empty())
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		size_t n = std::min(seg.size(), prefix.size());
		ଯଦି (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ TwinePool::name_equal(IdString ref, std::string_view name) ଅଚଳ
{
	ଫେରନ୍ତୁ str_size(ref) == name.size() && begins_with(ref, name);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ଯେତେବେଳେ (ସତ୍ୟ) {
		std::string_view x = sa.peek(), y = sb.peek();
		ଯଦି (x.empty() || y.empty())
			ଫେରନ୍ତୁ x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		ଯଦି (ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ଫେରନ୍ତୁ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ଅଚଳ {
	ଫେରନ୍ତୁ find_hashed(hash, [&](IdString ref) {
		ଅଚଳ Twine_ଗଣ୍ଠି &t = (*ଏହି)[ref];
		ଯଦି (t == Twine_ଗଣ୍ଠି::ଚାବି_2{prefix, text})
			ଫେରନ୍ତୁ ସତ୍ୟ;
		IdString head = prefix == Twine_ଗଣ୍ଠି::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*ଏହି, ref), sb(*ଏହି, head, text);
		ଫେରନ୍ତୁ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ଅଚଳ TwinePool& src, IdString ref) ଅଚଳ {
	ଯଦି (ref == IdString::Null || ID::is_static(ref))
		ଫେରନ୍ତୁ ref;
	IdString idx = ref.untag();
	ଅଚଳ Twine_ଗଣ୍ଠି& t = src[idx];
	ଯଦି (t.is_dead())
		ଫେରନ୍ତୁ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*ଏହି, cand), sb(src, idx);
		ଫେରନ୍ତୁ compare_segments(sa, sb) == 0;
	});
	ଫେରନ୍ତୁ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ଅଚଳ {
	ଅଚଳ Twine_ଗଣ୍ଠି &t = (*ଏହି)[ref];
	ଯଦି (!t.is_suffix())
		ଫେରନ୍ତୁ IdString::Null;
	ଫେରନ୍ତୁ stamp(t.prefix().tag(ref.isPublic()));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା TwinePool::compare_by_name(IdString a, IdString b) ଅଚଳ
{
	ଯଦି (a == b)
		ଫେରନ୍ତୁ 0;
	ଯଦି (a == IdString::Null)
		ଫେରନ୍ତୁ -1;
	ଯଦି (b == IdString::Null)
		ଫେରନ୍ତୁ 1;

	ଯଦି (a.isPublic() == b.isPublic()) {
		ଅଚଳ Twine_ଗଣ୍ଠି &ta = (*ଏହି)[a.untag()];
		ଅଚଳ Twine_ଗଣ୍ଠି &tb = (*ଏହି)[b.untag()];
		ଯଦି (ta.is_leaf() && tb.is_leaf())
			ଫେରନ୍ତୁ ta.text().compare(tb.text());
		ଯଦି (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ଫେରନ୍ତୁ ta.text().compare(tb.text());
	}

	TwineSegments sa(*ଏହି, a), sb(*ଏହି, b);
	ଫେରନ୍ତୁ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
