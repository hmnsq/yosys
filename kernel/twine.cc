#അന്തര്ഭാവ "kernel/twine.h"
#അന്തര്ഭാവ "kernel/hashcons_impl.h"
#അന്തര്ഭാവ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template ഘടന HashConsPool<TwinePool, Twine_കണ്ണി, IdString>;

std::vector<Twine_കണ്ണി> StaticTwines::nodes_;

ശൂന്യം StaticTwines::init() {
	എങ്കിൽ (ready())
		തിരികെ;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	വേണ്ടി (സ്ഥിരം അക്ഷരം *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

സ്ഥിരം Twine_കണ്ണി &StaticTwines::കണ്ണി(size_t idx) { തിരികെ nodes_[idx]; }
തർക്കം StaticTwines::ready() { തിരികെ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
പൂർണ്ണസംഖ്യ twine_gc_എണ്ണം;

std::string IdString::handle_token() സ്ഥിരം {
	തിരികെ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, തർക്കം &is_public) {
	എങ്കിൽ (token.substr(0, 5) == "$pub@") {
		is_public = സത്യം;
		തിരികെ 5;
	}
	എങ്കിൽ (token.substr(0, 6) == "$priv@") {
		is_public = അസത്യം;
		തിരികെ 6;
	}
	തിരികെ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	തിരികെ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	തിരികെ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

ശൂന്യം SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	എങ്കിൽ (len_ > INLINE_CAP) {
		ptr_ = പുതിയ അക്ഷരം[len_];
		memcpy(ptr_, content.data(), len_);
	} അല്ലെങ്കിൽ {
		memcpy(inl_, content.data(), len_);
	}
}

ശൂന്യം SmallString::release()
{
	എങ്കിൽ (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(സ്ഥിരം SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(സ്ഥിരം SmallString &other)
{
	എങ്കിൽ (ഇത് != &other)
		store(other.view());
	തിരികെ *ഇത്;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	എങ്കിൽ (ഇത് == &other)
		തിരികെ *ഇത്;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	തിരികെ *ഇത്;
}

SmallString::~SmallString() { release(); }

Twine_കണ്ണി::Twine_കണ്ണി(Twine_കണ്ണി &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_കണ്ണി &Twine_കണ്ണി::operator=(Twine_കണ്ണി &&other) noexcept
{
	എങ്കിൽ (ഇത് == &other)
		തിരികെ *ഇത്;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	തിരികെ *ഇത്;
}

std::string TwineSpec::content_str() സ്ഥിരം {
	എങ്കിൽ (auto *leaf = std::get_if<Leaf>(&data))
		തിരികെ leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	തിരികെ *autosfx.prefix + autosfx.tail;
}

സ്ഥിതം std::pair<std::string_view, തർക്കം> unescape_name(std::string_view name) {
	എങ്കിൽ (!name.empty() && name[0] == '\\')
		തിരികെ {name.substr(1), സത്യം};
	തിരികെ {name, അസത്യം};
}

സ്ഥിതം ശൂന്യം check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	വേണ്ടി (അക്ഷരം ch : name)
		എങ്കിൽ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(സ്ഥിരം TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(സ്ഥിരം TwinePool& other) {
	എങ്കിൽ (ഇത് == &other)
		തിരികെ *ഇത്;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	തിരികെ *ഇത്;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	എങ്കിൽ (ഇത് == &other)
		തിരികെ *ഇത്;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	തിരികെ *ഇത്;
}

തർക്കം TwinePool::owns(IdString ref) സ്ഥിരം {
	തിരികെ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) സ്ഥിരം {
	എങ്കിൽ (ref == IdString::Null || ID::is_static(ref))
		തിരികെ ref;
	തിരികെ ref.stamped(serial_);
}

ശൂന്യം TwinePool::check_owned(IdString ref) സ്ഥിരം {
	എങ്കിൽ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

സ്ഥിരം Twine_കണ്ണി& TwinePool::operator[](IdString ref) സ്ഥിരം {
	check_owned(ref);
	തിരികെ HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	വേണ്ടി (auto &it : auto_prefixes)
		roots.തിരുകുക(it.second);
	തിരികെ HashConsPool::gc(roots);
}

സ്ഥിരം Twine_കണ്ണി& TwinePool::static_കണ്ണി(size_t idx) { തിരികെ StaticTwines::കണ്ണി(idx); }
ശൂന്യം TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	സ്ഥിതം size_t counter = 0;
	size_t serial = ++counter;
	എങ്കിൽ constexpr (IdString::MAX_SERIAL == 0)
		തിരികെ serial;
	അല്ലെങ്കിൽ
		തിരികെ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_കണ്ണി t) {
	എങ്കിൽ (free_list.empty() && STATIC_എണ്ണം + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_എണ്ണം);
	തിരികെ HashConsPool::add_inner(std::move(t));
}

ശൂന്യം TwinePool::dump(IdString ref, std::ostream& os) സ്ഥിരം {
	സ്ഥിരം Twine_കണ്ണി& twine = (*ഇത്)[ref];
	തിരഞ്ഞെടുപ്പ് (twine.kind()) {
	അവസ്ഥ Twine_കണ്ണി::Kind::Dead:
		os << "Dead()";
		നിർത്തുക;
	അവസ്ഥ Twine_കണ്ണി::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		നിർത്തുക;
	അവസ്ഥ Twine_കണ്ണി::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		നിർത്തുക;
	}
	എങ്കിൽ (ref.isPublic())
		os << " pub";
}

ശൂന്യം TwinePool::append_str(IdString ref, std::string& out) സ്ഥിരം {
	എങ്കിൽ (ref == IdString::Null)
		തിരികെ;
	എങ്കിൽ (ref.isPublic())
		out += '\\';
	സ്ഥിരം Twine_കണ്ണി& twine = (*ഇത്)[ref];
	തിരഞ്ഞെടുപ്പ് (twine.kind()) {
	അവസ്ഥ Twine_കണ്ണി::Kind::Dead:
		നിർത്തുക;
	അവസ്ഥ Twine_കണ്ണി::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	അവസ്ഥ Twine_കണ്ണി::Kind::Leaf:
		out += twine.text();
		നിർത്തുക;
	}
}

std::string TwinePool::str(IdString ref) സ്ഥിരം {
	std::string out;
	append_str(ref, out);
	തിരികെ out;
}

std::string TwinePool::unescaped_str(IdString ref) സ്ഥിരം {
	തിരികെ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) സ്ഥിരം {
	uint32_t hash = prefix == Twine_കണ്ണി::NO_PREFIX ? 0 : (*ഇത്)[IdString(prefix)].hash();
	തിരികെ Twine_കണ്ണി::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) സ്ഥിരം {
	തിരികെ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	എങ്കിൽ (ref != IdString::Null)
		തിരികെ ref;
	തിരികെ add_inner(Twine_കണ്ണി{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) സ്ഥിരം {
	എങ്കിൽ (name.empty())
		തിരികെ IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	തിരികെ stamp(find_content(Twine_കണ്ണി::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) സ്ഥിരം {
	എങ്കിൽ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		തിരികെ stamp(find_content(Twine_കണ്ണി::NO_PREFIX, t.content_str()));
	എങ്കിൽ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		തിരികെ stamp(find_content(Twine_കണ്ണി::NO_PREFIX, leaf->s));
	സ്ഥിരം TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	തിരികെ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	എങ്കിൽ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		തിരികെ add(auto_prefix(ap->prefix), ap->tail);
	എങ്കിൽ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		തിരികെ stamp(intern(Twine_കണ്ണി::NO_PREFIX, leaf->s));
	സ്ഥിരം TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	തിരികെ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(സ്ഥിരം std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	എങ്കിൽ (it != auto_prefixes.end())
		തിരികെ it->second;
	IdString ref = intern(Twine_കണ്ണി::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	തിരികെ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	തിരികെ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	എങ്കിൽ (s.empty())
		തിരികെ IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	തിരികെ stamp(intern(Twine_കണ്ണി::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(സ്ഥിരം TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	എങ്കിൽ (found != IdString::Null || ref == IdString::Null)
		തിരികെ found;
	സ്ഥിരം Twine_കണ്ണി& t = src[ref.untag()];
	എങ്കിൽ (t.is_dead())
		തിരികെ IdString::Null;
	uint32_t prefix = Twine_കണ്ണി::NO_PREFIX;
	എങ്കിൽ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	തിരികെ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) സ്ഥിരം {
	തിരികെ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) സ്ഥിരം {
	എങ്കിൽ (token.size() < 2 || token[0] != '#')
		തിരികെ IdString::Null;
	size_t sep = token.find(':');
	എങ്കിൽ (sep == std::string_view::npos)
		തിരികെ IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 2; i++) {
		എങ്കിൽ (parts[i].empty())
			തിരികെ IdString::Null;
		വേണ്ടി (അക്ഷരം c : parts[i]) {
			എങ്കിൽ (c < '0' || c > '9')
				തിരികെ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	എങ്കിൽ (fields[0] != serial_)
		തിരികെ IdString::Null;
	IdString ref(fields[1]);
	എങ്കിൽ (ref == IdString::Null)
		തിരികെ IdString::Null;
	തിരികെ is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ഘടന TwineSegments {
	സ്ഥിതം constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(സ്ഥിരം TwinePool &pool, IdString ref);
	TwineSegments(സ്ഥിരം TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	ശൂന്യം advance(size_t n);
	size_t total_size() സ്ഥിരം;
സ്വകാര്യ:
	ശൂന്യം തള്ളുക(std::string_view seg);
	std::string_view *segs();
	സ്ഥിരം std::string_view *segs() സ്ഥിരം;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t എണ്ണം = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(സ്ഥിരം TwinePool &pool, IdString ref)
{
	എങ്കിൽ (ref == IdString::Null)
		തിരികെ;
	വേണ്ടി (IdString cur = ref.untag(); ;) {
		സ്ഥിരം Twine_കണ്ണി &t = pool[cur];
		തിരഞ്ഞെടുപ്പ് (t.kind()) {
		അവസ്ഥ Twine_കണ്ണി::Kind::Suffix:
			തള്ളുക(t.text());
			cur = t.prefix();
			തുടരുക;
		അവസ്ഥ Twine_കണ്ണി::Kind::Leaf:
			തള്ളുക(t.text());
			നിർത്തുക;
		അവസ്ഥ Twine_കണ്ണി::Kind::Dead:
			നിർത്തുക;
		}
		നിർത്തുക;
	}
	എങ്കിൽ (ref.isPublic())
		തള്ളുക("\\");
	std::reverse(segs(), segs() + എണ്ണം);
}

TwineSegments::TwineSegments(സ്ഥിരം TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	തള്ളുക(tail);
}

std::string_view TwineSegments::peek()
{
	ആകുമ്പോൾ (pos < എണ്ണം) {
		std::string_view seg = segs()[pos];
		എങ്കിൽ (off < seg.size())
			തിരികെ seg.substr(off);
		pos++;
		off = 0;
	}
	തിരികെ {};
}

ശൂന്യം TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() സ്ഥിരം
{
	size_t total = 0;
	വേണ്ടി (size_t i = 0; i < എണ്ണം; i++)
		total += segs()[i].size();
	തിരികെ total;
}

ശൂന്യം TwineSegments::തള്ളുക(std::string_view seg)
{
	എങ്കിൽ (spill.empty() && എണ്ണം < SEGMENT_INLINE_DEPTH) {
		inline_segs[എണ്ണം++] = seg;
		തിരികെ;
	}
	എങ്കിൽ (spill.empty())
		spill.assign(inline_segs, inline_segs + എണ്ണം);
	spill.തള്ളുക_back(seg);
	എണ്ണം++;
}

std::string_view *TwineSegments::segs() { തിരികെ spill.empty() ? inline_segs : spill.data(); }
സ്ഥിരം std::string_view *TwineSegments::segs() സ്ഥിരം { തിരികെ spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) സ്ഥിരം
{
	തിരികെ TwineSegments(*ഇത്, ref).total_size();
}

തർക്കം TwinePool::begins_with(IdString ref, std::string_view prefix) സ്ഥിരം
{
	TwineSegments segs(*ഇത്, ref);
	ആകുമ്പോൾ (!prefix.empty()) {
		std::string_view seg = segs.peek();
		എങ്കിൽ (seg.empty())
			തിരികെ അസത്യം;
		size_t n = std::min(seg.size(), prefix.size());
		എങ്കിൽ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			തിരികെ അസത്യം;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	തിരികെ സത്യം;
}

തർക്കം TwinePool::name_equal(IdString ref, std::string_view name) സ്ഥിരം
{
	തിരികെ str_size(ref) == name.size() && begins_with(ref, name);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ആകുമ്പോൾ (സത്യം) {
		std::string_view x = sa.peek(), y = sb.peek();
		എങ്കിൽ (x.empty() || y.empty())
			തിരികെ x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		എങ്കിൽ (പൂർണ്ണസംഖ്യ diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			തിരികെ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) സ്ഥിരം {
	തിരികെ find_hashed(hash, [&](IdString ref) {
		സ്ഥിരം Twine_കണ്ണി &t = (*ഇത്)[ref];
		എങ്കിൽ (t == Twine_കണ്ണി::താക്കോൽ_2{prefix, text})
			തിരികെ സത്യം;
		IdString head = prefix == Twine_കണ്ണി::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*ഇത്, ref), sb(*ഇത്, head, text);
		തിരികെ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(സ്ഥിരം TwinePool& src, IdString ref) സ്ഥിരം {
	എങ്കിൽ (ref == IdString::Null || ID::is_static(ref))
		തിരികെ ref;
	IdString idx = ref.untag();
	സ്ഥിരം Twine_കണ്ണി& t = src[idx];
	എങ്കിൽ (t.is_dead())
		തിരികെ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*ഇത്, cand), sb(src, idx);
		തിരികെ compare_segments(sa, sb) == 0;
	});
	തിരികെ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) സ്ഥിരം {
	സ്ഥിരം Twine_കണ്ണി &t = (*ഇത്)[ref];
	എങ്കിൽ (!t.is_suffix())
		തിരികെ IdString::Null;
	തിരികെ stamp(t.prefix().tag(ref.isPublic()));
}

പൂർണ്ണസംഖ്യ TwinePool::compare_by_name(IdString a, IdString b) സ്ഥിരം
{
	എങ്കിൽ (a == b)
		തിരികെ 0;
	എങ്കിൽ (a == IdString::Null)
		തിരികെ -1;
	എങ്കിൽ (b == IdString::Null)
		തിരികെ 1;

	എങ്കിൽ (a.isPublic() == b.isPublic()) {
		സ്ഥിരം Twine_കണ്ണി &ta = (*ഇത്)[a.untag()];
		സ്ഥിരം Twine_കണ്ണി &tb = (*ഇത്)[b.untag()];
		എങ്കിൽ (ta.is_leaf() && tb.is_leaf())
			തിരികെ ta.text().compare(tb.text());
		എങ്കിൽ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			തിരികെ ta.text().compare(tb.text());
	}

	TwineSegments sa(*ഇത്, a), sb(*ഇത്, b);
	തിരികെ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
