#ಅನ್ತರ್ಭಾವ "kernel/twine.h"
#ಅನ್ತರ್ಭಾವ "kernel/hashcons_impl.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template ಸಂರಚನಾ HashConsPool<TwinePool, Twine_ಗಂಟು, IdString>;

std::vector<Twine_ಗಂಟು> StaticTwines::nodes_;

ಶೂನ್ಯ StaticTwines::init() {
	ಯದಿ (ready())
		ಪ್ರತ್ಯಾಗಮ;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ಧ್ರುವ Twine_ಗಂಟು &StaticTwines::ಗಂಟು(size_t idx) { ಪ್ರತ್ಯಾಗಮ nodes_[idx]; }
ತಾರ್ಕಿಕ StaticTwines::ready() { ಪ್ರತ್ಯಾಗಮ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
ಪೂರ್ಣಾಙ್ಕ twine_gc_ಎಣಿಕೆ;

std::string IdString::handle_token() ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, ತಾರ್ಕಿಕ &is_public) {
	ಯದಿ (token.substr(0, 5) == "$pub@") {
		is_public = ಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ 5;
	}
	ಯದಿ (token.substr(0, 6) == "$priv@") {
		is_public = ಅಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ 6;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ಪ್ರತ್ಯಾಗಮ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ಪ್ರತ್ಯಾಗಮ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

ಶೂನ್ಯ SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	ಯದಿ (len_ > INLINE_CAP) {
		ptr_ = ನೂತನ ವರ್ಣ[len_];
		memcpy(ptr_, content.data(), len_);
	} ಅನ್ಯಥಾ {
		memcpy(inl_, content.data(), len_);
	}
}

ಶೂನ್ಯ SmallString::release()
{
	ಯದಿ (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ಧ್ರುವ SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ಧ್ರುವ SmallString &other)
{
	ಯದಿ (ಏತತ್ != &other)
		store(other.view());
	ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	ಯದಿ (ಏತತ್ == &other)
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

SmallString::~SmallString() { release(); }

Twine_ಗಂಟು::Twine_ಗಂಟು(Twine_ಗಂಟು &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_ಗಂಟು &Twine_ಗಂಟು::operator=(Twine_ಗಂಟು &&other) noexcept
{
	ಯದಿ (ಏತತ್ == &other)
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

std::string TwineSpec::content_str() ಧ್ರುವ {
	ಯದಿ (auto *leaf = std::get_if<Leaf>(&data))
		ಪ್ರತ್ಯಾಗಮ leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ಪ್ರತ್ಯಾಗಮ *autosfx.prefix + autosfx.tail;
}

ಸ್ಥಿರ std::pair<std::string_view, ತಾರ್ಕಿಕ> unescape_name(std::string_view name) {
	ಯದಿ (!name.empty() && name[0] == '\\')
		ಪ್ರತ್ಯಾಗಮ {name.substr(1), ಸತ್ಯ};
	ಪ್ರತ್ಯಾಗಮ {name, ಅಸತ್ಯ};
}

ಸ್ಥಿರ ಶೂನ್ಯ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	ಪ್ರತಿ (ವರ್ಣ ch : name)
		ಯದಿ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ಧ್ರುವ TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ಧ್ರುವ TwinePool& other) {
	ಯದಿ (ಏತತ್ == &other)
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	ಯದಿ (ಏತತ್ == &other)
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ತಾರ್ಕಿಕ TwinePool::owns(IdString ref) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ಧ್ರುವ {
	ಯದಿ (ref == IdString::Null || ID::is_static(ref))
		ಪ್ರತ್ಯಾಗಮ ref;
	ಪ್ರತ್ಯಾಗಮ ref.stamped(serial_);
}

ಶೂನ್ಯ TwinePool::check_owned(IdString ref) ಧ್ರುವ {
	ಯದಿ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ಧ್ರುವ Twine_ಗಂಟು& TwinePool::operator[](IdString ref) ಧ್ರುವ {
	check_owned(ref);
	ಪ್ರತ್ಯಾಗಮ HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	ಪ್ರತಿ (auto &it : auto_prefixes)
		roots.ಸೇರಿಸು(it.second);
	ಪ್ರತ್ಯಾಗಮ HashConsPool::gc(roots);
}

ಧ್ರುವ Twine_ಗಂಟು& TwinePool::static_ಗಂಟು(size_t idx) { ಪ್ರತ್ಯಾಗಮ StaticTwines::ಗಂಟು(idx); }
ಶೂನ್ಯ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	ಸ್ಥಿರ size_t counter = 0;
	size_t serial = ++counter;
	ಯದಿ constexpr (IdString::MAX_SERIAL == 0)
		ಪ್ರತ್ಯಾಗಮ serial;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_ಗಂಟು t) {
	ಯದಿ (free_list.empty() && STATIC_ಎಣಿಕೆ + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_ಎಣಿಕೆ);
	ಪ್ರತ್ಯಾಗಮ HashConsPool::add_inner(std::move(t));
}

ಶೂನ್ಯ TwinePool::dump(IdString ref, std::ostream& os) ಧ್ರುವ {
	ಧ್ರುವ Twine_ಗಂಟು& twine = (*ಏತತ್)[ref];
	ವಿಕಲ್ಪ (twine.kind()) {
	ಪಕ್ಷ Twine_ಗಂಟು::Kind::Dead:
		os << "Dead()";
		ಭಙ್ಗ;
	ಪಕ್ಷ Twine_ಗಂಟು::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		ಭಙ್ಗ;
	ಪಕ್ಷ Twine_ಗಂಟು::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		ಭಙ್ಗ;
	}
	ಯದಿ (ref.isPublic())
		os << " pub";
}

ಶೂನ್ಯ TwinePool::append_str(IdString ref, std::string& out) ಧ್ರುವ {
	ಯದಿ (ref == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (ref.isPublic())
		out += '\\';
	ಧ್ರುವ Twine_ಗಂಟು& twine = (*ಏತತ್)[ref];
	ವಿಕಲ್ಪ (twine.kind()) {
	ಪಕ್ಷ Twine_ಗಂಟು::Kind::Dead:
		ಭಙ್ಗ;
	ಪಕ್ಷ Twine_ಗಂಟು::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	ಪಕ್ಷ Twine_ಗಂಟು::Kind::Leaf:
		out += twine.text();
		ಭಙ್ಗ;
	}
}

std::string TwinePool::str(IdString ref) ಧ್ರುವ {
	std::string out;
	append_str(ref, out);
	ಪ್ರತ್ಯಾಗಮ out;
}

std::string TwinePool::unescaped_str(IdString ref) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ಧ್ರುವ {
	uint32_t hash = prefix == Twine_ಗಂಟು::NO_PREFIX ? 0 : (*ಏತತ್)[IdString(prefix)].hash();
	ಪ್ರತ್ಯಾಗಮ Twine_ಗಂಟು::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	ಯದಿ (ref != IdString::Null)
		ಪ್ರತ್ಯಾಗಮ ref;
	ಪ್ರತ್ಯಾಗಮ add_inner(Twine_ಗಂಟು{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ಧ್ರುವ {
	ಯದಿ (name.empty())
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ಪ್ರತ್ಯಾಗಮ stamp(find_content(Twine_ಗಂಟು::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ಧ್ರುವ {
	ಯದಿ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ಪ್ರತ್ಯಾಗಮ stamp(find_content(Twine_ಗಂಟು::NO_PREFIX, t.content_str()));
	ಯದಿ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ಪ್ರತ್ಯಾಗಮ stamp(find_content(Twine_ಗಂಟು::NO_PREFIX, leaf->s));
	ಧ್ರುವ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ಪ್ರತ್ಯಾಗಮ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	ಯದಿ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ಪ್ರತ್ಯಾಗಮ add(auto_prefix(ap->prefix), ap->tail);
	ಯದಿ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ಪ್ರತ್ಯಾಗಮ stamp(intern(Twine_ಗಂಟು::NO_PREFIX, leaf->s));
	ಧ್ರುವ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ಪ್ರತ್ಯಾಗಮ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ಧ್ರುವ std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	ಯದಿ (it != auto_prefixes.end())
		ಪ್ರತ್ಯಾಗಮ it->second;
	IdString ref = intern(Twine_ಗಂಟು::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ಪ್ರತ್ಯಾಗಮ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ಪ್ರತ್ಯಾಗಮ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	ಯದಿ (s.empty())
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ಪ್ರತ್ಯಾಗಮ stamp(intern(Twine_ಗಂಟು::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ಧ್ರುವ TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	ಯದಿ (found != IdString::Null || ref == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ found;
	ಧ್ರುವ Twine_ಗಂಟು& t = src[ref.untag()];
	ಯದಿ (t.is_dead())
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	uint32_t prefix = Twine_ಗಂಟು::NO_PREFIX;
	ಯದಿ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ಪ್ರತ್ಯಾಗಮ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ಧ್ರುವ {
	ಯದಿ (token.size() < 2 || token[0] != '#')
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	size_t sep = token.find(':');
	ಯದಿ (sep == std::string_view::npos)
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 2; i++) {
		ಯದಿ (parts[i].empty())
			ಪ್ರತ್ಯಾಗಮ IdString::Null;
		ಪ್ರತಿ (ವರ್ಣ c : parts[i]) {
			ಯದಿ (c < '0' || c > '9')
				ಪ್ರತ್ಯಾಗಮ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ಯದಿ (fields[0] != serial_)
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	IdString ref(fields[1]);
	ಯದಿ (ref == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	ಪ್ರತ್ಯಾಗಮ is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ಸಂರಚನಾ TwineSegments {
	ಸ್ಥಿರ constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ಧ್ರುವ TwinePool &pool, IdString ref);
	TwineSegments(ಧ್ರುವ TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	ಶೂನ್ಯ advance(size_t n);
	size_t total_size() ಧ್ರುವ;
ಗುಪ್ತ:
	ಶೂನ್ಯ ತಳ್ಳು(std::string_view seg);
	std::string_view *segs();
	ಧ್ರುವ std::string_view *segs() ಧ್ರುವ;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t ಎಣಿಕೆ = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ಧ್ರುವ TwinePool &pool, IdString ref)
{
	ಯದಿ (ref == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ;
	ಪ್ರತಿ (IdString cur = ref.untag(); ;) {
		ಧ್ರುವ Twine_ಗಂಟು &t = pool[cur];
		ವಿಕಲ್ಪ (t.kind()) {
		ಪಕ್ಷ Twine_ಗಂಟು::Kind::Suffix:
			ತಳ್ಳು(t.text());
			cur = t.prefix();
			ಅನುವರ್ತ;
		ಪಕ್ಷ Twine_ಗಂಟು::Kind::Leaf:
			ತಳ್ಳು(t.text());
			ಭಙ್ಗ;
		ಪಕ್ಷ Twine_ಗಂಟು::Kind::Dead:
			ಭಙ್ಗ;
		}
		ಭಙ್ಗ;
	}
	ಯದಿ (ref.isPublic())
		ತಳ್ಳು("\\");
	std::reverse(segs(), segs() + ಎಣಿಕೆ);
}

TwineSegments::TwineSegments(ಧ್ರುವ TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	ತಳ್ಳು(tail);
}

std::string_view TwineSegments::peek()
{
	ಯಾವತ್ (pos < ಎಣಿಕೆ) {
		std::string_view seg = segs()[pos];
		ಯದಿ (off < seg.size())
			ಪ್ರತ್ಯಾಗಮ seg.substr(off);
		pos++;
		off = 0;
	}
	ಪ್ರತ್ಯಾಗಮ {};
}

ಶೂನ್ಯ TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ಧ್ರುವ
{
	size_t total = 0;
	ಪ್ರತಿ (size_t i = 0; i < ಎಣಿಕೆ; i++)
		total += segs()[i].size();
	ಪ್ರತ್ಯಾಗಮ total;
}

ಶೂನ್ಯ TwineSegments::ತಳ್ಳು(std::string_view seg)
{
	ಯದಿ (spill.empty() && ಎಣಿಕೆ < SEGMENT_INLINE_DEPTH) {
		inline_segs[ಎಣಿಕೆ++] = seg;
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (spill.empty())
		spill.assign(inline_segs, inline_segs + ಎಣಿಕೆ);
	spill.ತಳ್ಳು_back(seg);
	ಎಣಿಕೆ++;
}

std::string_view *TwineSegments::segs() { ಪ್ರತ್ಯಾಗಮ spill.empty() ? inline_segs : spill.data(); }
ಧ್ರುವ std::string_view *TwineSegments::segs() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ TwineSegments(*ಏತತ್, ref).total_size();
}

ತಾರ್ಕಿಕ TwinePool::begins_with(IdString ref, std::string_view prefix) ಧ್ರುವ
{
	TwineSegments segs(*ಏತತ್, ref);
	ಯಾವತ್ (!prefix.empty()) {
		std::string_view seg = segs.peek();
		ಯದಿ (seg.empty())
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		size_t n = std::min(seg.size(), prefix.size());
		ಯದಿ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ TwinePool::name_equal(IdString ref, std::string_view name) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ str_size(ref) == name.size() && begins_with(ref, name);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	ಯಾವತ್ (ಸತ್ಯ) {
		std::string_view x = sa.peek(), y = sb.peek();
		ಯದಿ (x.empty() || y.empty())
			ಪ್ರತ್ಯಾಗಮ x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		ಯದಿ (ಪೂರ್ಣಾಙ್ಕ diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ಪ್ರತ್ಯಾಗಮ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ find_hashed(hash, [&](IdString ref) {
		ಧ್ರುವ Twine_ಗಂಟು &t = (*ಏತತ್)[ref];
		ಯದಿ (t == Twine_ಗಂಟು::ಕೀಲಿ_2{prefix, text})
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		IdString head = prefix == Twine_ಗಂಟು::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*ಏತತ್, ref), sb(*ಏತತ್, head, text);
		ಪ್ರತ್ಯಾಗಮ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ಧ್ರುವ TwinePool& src, IdString ref) ಧ್ರುವ {
	ಯದಿ (ref == IdString::Null || ID::is_static(ref))
		ಪ್ರತ್ಯಾಗಮ ref;
	IdString idx = ref.untag();
	ಧ್ರುವ Twine_ಗಂಟು& t = src[idx];
	ಯದಿ (t.is_dead())
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*ಏತತ್, cand), sb(src, idx);
		ಪ್ರತ್ಯಾಗಮ compare_segments(sa, sb) == 0;
	});
	ಪ್ರತ್ಯಾಗಮ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ಧ್ರುವ {
	ಧ್ರುವ Twine_ಗಂಟು &t = (*ಏತತ್)[ref];
	ಯದಿ (!t.is_suffix())
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	ಪ್ರತ್ಯಾಗಮ stamp(t.prefix().tag(ref.isPublic()));
}

ಪೂರ್ಣಾಙ್ಕ TwinePool::compare_by_name(IdString a, IdString b) ಧ್ರುವ
{
	ಯದಿ (a == b)
		ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (a == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (b == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ 1;

	ಯದಿ (a.isPublic() == b.isPublic()) {
		ಧ್ರುವ Twine_ಗಂಟು &ta = (*ಏತತ್)[a.untag()];
		ಧ್ರುವ Twine_ಗಂಟು &tb = (*ಏತತ್)[b.untag()];
		ಯದಿ (ta.is_leaf() && tb.is_leaf())
			ಪ್ರತ್ಯಾಗಮ ta.text().compare(tb.text());
		ಯದಿ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ಪ್ರತ್ಯಾಗಮ ta.text().compare(tb.text());
	}

	TwineSegments sa(*ಏತತ್, a), sb(*ಏತತ್, b);
	ಪ್ರತ್ಯಾಗಮ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
