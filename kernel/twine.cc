#અન્તર્ભાવ "kernel/twine.h"
#અન્તર્ભાવ "kernel/hashcons_impl.h"
#અન્તર્ભાવ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template સંરચના HashConsPool<TwinePool, Twine_ગાંઠ, IdString>;

std::vector<Twine_ગાંઠ> StaticTwines::nodes_;

શૂન્ય StaticTwines::init() {
	જો (ready())
		પાછા;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	માટે (અચલ અક્ષર *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

અચલ Twine_ગાંઠ &StaticTwines::ગાંઠ(size_t idx) { પાછા nodes_[idx]; }
તાર્કિક StaticTwines::ready() { પાછા nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
પૂર્ણાંક twine_gc_ગણતરી;

std::string IdString::handle_token() અચલ {
	પાછા stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, તાર્કિક &is_public) {
	જો (token.substr(0, 5) == "$pub@") {
		is_public = સાચું;
		પાછા 5;
	}
	જો (token.substr(0, 6) == "$priv@") {
		is_public = ખોટું;
		પાછા 6;
	}
	પાછા 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	પાછા result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	પાછા static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

શૂન્ય SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	જો (len_ > INLINE_CAP) {
		ptr_ = નવું અક્ષર[len_];
		memcpy(ptr_, content.data(), len_);
	} નહીંતર {
		memcpy(inl_, content.data(), len_);
	}
}

શૂન્ય SmallString::release()
{
	જો (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(અચલ SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(અચલ SmallString &other)
{
	જો (આ != &other)
		store(other.view());
	પાછા *આ;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	જો (આ == &other)
		પાછા *આ;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	પાછા *આ;
}

SmallString::~SmallString() { release(); }

Twine_ગાંઠ::Twine_ગાંઠ(Twine_ગાંઠ &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_ગાંઠ &Twine_ગાંઠ::operator=(Twine_ગાંઠ &&other) noexcept
{
	જો (આ == &other)
		પાછા *આ;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	પાછા *આ;
}

std::string TwineSpec::content_str() અચલ {
	જો (auto *leaf = std::get_if<Leaf>(&data))
		પાછા leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	પાછા *autosfx.prefix + autosfx.tail;
}

સ્થિર std::pair<std::string_view, તાર્કિક> unescape_name(std::string_view name) {
	જો (!name.empty() && name[0] == '\\')
		પાછા {name.substr(1), સાચું};
	પાછા {name, ખોટું};
}

સ્થિર શૂન્ય check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	માટે (અક્ષર ch : name)
		જો ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(અચલ TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(અચલ TwinePool& other) {
	જો (આ == &other)
		પાછા *આ;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	પાછા *આ;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	જો (આ == &other)
		પાછા *આ;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	પાછા *આ;
}

તાર્કિક TwinePool::owns(IdString ref) અચલ {
	પાછા ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) અચલ {
	જો (ref == IdString::Null || ID::is_static(ref))
		પાછા ref;
	પાછા ref.stamped(serial_);
}

શૂન્ય TwinePool::check_owned(IdString ref) અચલ {
	જો constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

અચલ Twine_ગાંઠ& TwinePool::operator[](IdString ref) અચલ {
	check_owned(ref);
	પાછા HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	માટે (auto &it : auto_prefixes)
		roots.ઉમેરો(it.second);
	પાછા HashConsPool::gc(roots);
}

અચલ Twine_ગાંઠ& TwinePool::static_ગાંઠ(size_t idx) { પાછા StaticTwines::ગાંઠ(idx); }
શૂન્ય TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	સ્થિર size_t counter = 0;
	size_t serial = ++counter;
	જો constexpr (IdString::MAX_SERIAL == 0)
		પાછા serial;
	નહીંતર
		પાછા serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_ગાંઠ t) {
	જો (free_list.empty() && STATIC_ગણતરી + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_ગણતરી);
	પાછા HashConsPool::add_inner(std::move(t));
}

શૂન્ય TwinePool::dump(IdString ref, std::ostream& os) અચલ {
	અચલ Twine_ગાંઠ& twine = (*આ)[ref];
	પસંદગી (twine.kind()) {
	સ્થિતિ Twine_ગાંઠ::Kind::Dead:
		os << "Dead()";
		તોડો;
	સ્થિતિ Twine_ગાંઠ::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		તોડો;
	સ્થિતિ Twine_ગાંઠ::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		તોડો;
	}
	જો (ref.isPublic())
		os << " pub";
}

શૂન્ય TwinePool::append_str(IdString ref, std::string& out) અચલ {
	જો (ref == IdString::Null)
		પાછા;
	જો (ref.isPublic())
		out += '\\';
	અચલ Twine_ગાંઠ& twine = (*આ)[ref];
	પસંદગી (twine.kind()) {
	સ્થિતિ Twine_ગાંઠ::Kind::Dead:
		તોડો;
	સ્થિતિ Twine_ગાંઠ::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	સ્થિતિ Twine_ગાંઠ::Kind::Leaf:
		out += twine.text();
		તોડો;
	}
}

std::string TwinePool::str(IdString ref) અચલ {
	std::string out;
	append_str(ref, out);
	પાછા out;
}

std::string TwinePool::unescaped_str(IdString ref) અચલ {
	પાછા str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) અચલ {
	uint32_t hash = prefix == Twine_ગાંઠ::NO_PREFIX ? 0 : (*આ)[IdString(prefix)].hash();
	પાછા Twine_ગાંઠ::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) અચલ {
	પાછા find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	જો (ref != IdString::Null)
		પાછા ref;
	પાછા add_inner(Twine_ગાંઠ{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) અચલ {
	જો (name.empty())
		પાછા IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	પાછા stamp(find_content(Twine_ગાંઠ::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) અચલ {
	જો (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		પાછા stamp(find_content(Twine_ગાંઠ::NO_PREFIX, t.content_str()));
	જો (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		પાછા stamp(find_content(Twine_ગાંઠ::NO_PREFIX, leaf->s));
	અચલ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	પાછા stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	જો (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		પાછા add(auto_prefix(ap->prefix), ap->tail);
	જો (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		પાછા stamp(intern(Twine_ગાંઠ::NO_PREFIX, leaf->s));
	અચલ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	પાછા add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(અચલ std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	જો (it != auto_prefixes.end())
		પાછા it->second;
	IdString ref = intern(Twine_ગાંઠ::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	પાછા ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	પાછા stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	જો (s.empty())
		પાછા IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	પાછા stamp(intern(Twine_ગાંઠ::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(અચલ TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	જો (found != IdString::Null || ref == IdString::Null)
		પાછા found;
	અચલ Twine_ગાંઠ& t = src[ref.untag()];
	જો (t.is_dead())
		પાછા IdString::Null;
	uint32_t prefix = Twine_ગાંઠ::NO_PREFIX;
	જો (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	પાછા stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) અચલ {
	પાછા "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) અચલ {
	જો (token.size() < 2 || token[0] != '#')
		પાછા IdString::Null;
	size_t sep = token.find(':');
	જો (sep == std::string_view::npos)
		પાછા IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	માટે (પૂર્ણાંક i = 0; i < 2; i++) {
		જો (parts[i].empty())
			પાછા IdString::Null;
		માટે (અક્ષર c : parts[i]) {
			જો (c < '0' || c > '9')
				પાછા IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	જો (fields[0] != serial_)
		પાછા IdString::Null;
	IdString ref(fields[1]);
	જો (ref == IdString::Null)
		પાછા IdString::Null;
	પાછા is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
સંરચના TwineSegments {
	સ્થિર constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(અચલ TwinePool &pool, IdString ref);
	TwineSegments(અચલ TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	શૂન્ય advance(size_t n);
	size_t total_size() અચલ;
ખાનગી:
	શૂન્ય ધકેલો(std::string_view seg);
	std::string_view *segs();
	અચલ std::string_view *segs() અચલ;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t ગણતરી = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(અચલ TwinePool &pool, IdString ref)
{
	જો (ref == IdString::Null)
		પાછા;
	માટે (IdString cur = ref.untag(); ;) {
		અચલ Twine_ગાંઠ &t = pool[cur];
		પસંદગી (t.kind()) {
		સ્થિતિ Twine_ગાંઠ::Kind::Suffix:
			ધકેલો(t.text());
			cur = t.prefix();
			ચાલુ;
		સ્થિતિ Twine_ગાંઠ::Kind::Leaf:
			ધકેલો(t.text());
			તોડો;
		સ્થિતિ Twine_ગાંઠ::Kind::Dead:
			તોડો;
		}
		તોડો;
	}
	જો (ref.isPublic())
		ધકેલો("\\");
	std::reverse(segs(), segs() + ગણતરી);
}

TwineSegments::TwineSegments(અચલ TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	ધકેલો(tail);
}

std::string_view TwineSegments::peek()
{
	જ્યારે (pos < ગણતરી) {
		std::string_view seg = segs()[pos];
		જો (off < seg.size())
			પાછા seg.substr(off);
		pos++;
		off = 0;
	}
	પાછા {};
}

શૂન્ય TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() અચલ
{
	size_t total = 0;
	માટે (size_t i = 0; i < ગણતરી; i++)
		total += segs()[i].size();
	પાછા total;
}

શૂન્ય TwineSegments::ધકેલો(std::string_view seg)
{
	જો (spill.empty() && ગણતરી < SEGMENT_INLINE_DEPTH) {
		inline_segs[ગણતરી++] = seg;
		પાછા;
	}
	જો (spill.empty())
		spill.assign(inline_segs, inline_segs + ગણતરી);
	spill.ધકેલો_back(seg);
	ગણતરી++;
}

std::string_view *TwineSegments::segs() { પાછા spill.empty() ? inline_segs : spill.data(); }
અચલ std::string_view *TwineSegments::segs() અચલ { પાછા spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) અચલ
{
	પાછા TwineSegments(*આ, ref).total_size();
}

તાર્કિક TwinePool::begins_with(IdString ref, std::string_view prefix) અચલ
{
	TwineSegments segs(*આ, ref);
	જ્યારે (!prefix.empty()) {
		std::string_view seg = segs.peek();
		જો (seg.empty())
			પાછા ખોટું;
		size_t n = std::min(seg.size(), prefix.size());
		જો (std::memcmp(seg.data(), prefix.data(), n) != 0)
			પાછા ખોટું;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	પાછા સાચું;
}

તાર્કિક TwinePool::name_equal(IdString ref, std::string_view name) અચલ
{
	પાછા str_size(ref) == name.size() && begins_with(ref, name);
}

સ્થિર પૂર્ણાંક compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	જ્યારે (સાચું) {
		std::string_view x = sa.peek(), y = sb.peek();
		જો (x.empty() || y.empty())
			પાછા x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		જો (પૂર્ણાંક diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			પાછા diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) અચલ {
	પાછા find_hashed(hash, [&](IdString ref) {
		અચલ Twine_ગાંઠ &t = (*આ)[ref];
		જો (t == Twine_ગાંઠ::ચાવી_2{prefix, text})
			પાછા સાચું;
		IdString head = prefix == Twine_ગાંઠ::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*આ, ref), sb(*આ, head, text);
		પાછા compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(અચલ TwinePool& src, IdString ref) અચલ {
	જો (ref == IdString::Null || ID::is_static(ref))
		પાછા ref;
	IdString idx = ref.untag();
	અચલ Twine_ગાંઠ& t = src[idx];
	જો (t.is_dead())
		પાછા IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*આ, cand), sb(src, idx);
		પાછા compare_segments(sa, sb) == 0;
	});
	પાછા stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) અચલ {
	અચલ Twine_ગાંઠ &t = (*આ)[ref];
	જો (!t.is_suffix())
		પાછા IdString::Null;
	પાછા stamp(t.prefix().tag(ref.isPublic()));
}

પૂર્ણાંક TwinePool::compare_by_name(IdString a, IdString b) અચલ
{
	જો (a == b)
		પાછા 0;
	જો (a == IdString::Null)
		પાછા -1;
	જો (b == IdString::Null)
		પાછા 1;

	જો (a.isPublic() == b.isPublic()) {
		અચલ Twine_ગાંઠ &ta = (*આ)[a.untag()];
		અચલ Twine_ગાંઠ &tb = (*આ)[b.untag()];
		જો (ta.is_leaf() && tb.is_leaf())
			પાછા ta.text().compare(tb.text());
		જો (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			પાછા ta.text().compare(tb.text());
	}

	TwineSegments sa(*આ, a), sb(*આ, b);
	પાછા compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
