#अन्तर्भाव "kernel/twine.h"
#अन्तर्भाव "kernel/hashcons_impl.h"
#अन्तर्भाव "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template संरचना HashConsPool<TwinePool, Twine_ग्रन्थिः, IdString>;

std::vector<Twine_ग्रन्थिः> StaticTwines::nodes_;

शून्य StaticTwines::init() {
	जइ (ready())
		प्रत्यागम;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	प्रति (ध्रुव वर्ण *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ध्रुव Twine_ग्रन्थिः &StaticTwines::ग्रन्थिः(size_t idx) { प्रत्यागम nodes_[idx]; }
तार्किक StaticTwines::ready() { प्रत्यागम nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
पूर्णाङ्क twine_gc_गणनम्;

std::string IdString::handle_token() ध्रुव {
	प्रत्यागम stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, तार्किक &is_public) {
	जइ (token.substr(0, 5) == "$pub@") {
		is_public = सच्च;
		प्रत्यागम 5;
	}
	जइ (token.substr(0, 6) == "$priv@") {
		is_public = मिच्छ;
		प्रत्यागम 6;
	}
	प्रत्यागम 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	प्रत्यागम result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	प्रत्यागम static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

शून्य SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	जइ (len_ > INLINE_CAP) {
		ptr_ = नूतन वर्ण[len_];
		memcpy(ptr_, content.data(), len_);
	} अण्णहा {
		memcpy(inl_, content.data(), len_);
	}
}

शून्य SmallString::release()
{
	जइ (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ध्रुव SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ध्रुव SmallString &other)
{
	जइ (एतत् != &other)
		store(other.view());
	प्रत्यागम *एतत्;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	जइ (एतत् == &other)
		प्रत्यागम *एतत्;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	प्रत्यागम *एतत्;
}

SmallString::~SmallString() { release(); }

Twine_ग्रन्थिः::Twine_ग्रन्थिः(Twine_ग्रन्थिः &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_ग्रन्थिः &Twine_ग्रन्थिः::operator=(Twine_ग्रन्थिः &&other) noexcept
{
	जइ (एतत् == &other)
		प्रत्यागम *एतत्;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	प्रत्यागम *एतत्;
}

std::string TwineSpec::content_str() ध्रुव {
	जइ (auto *leaf = std::get_if<Leaf>(&data))
		प्रत्यागम leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	प्रत्यागम *autosfx.prefix + autosfx.tail;
}

स्थिर std::pair<std::string_view, तार्किक> unescape_name(std::string_view name) {
	जइ (!name.empty() && name[0] == '\\')
		प्रत्यागम {name.substr(1), सच्च};
	प्रत्यागम {name, मिच्छ};
}

स्थिर शून्य check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	प्रति (वर्ण ch : name)
		जइ ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ध्रुव TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ध्रुव TwinePool& other) {
	जइ (एतत् == &other)
		प्रत्यागम *एतत्;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	प्रत्यागम *एतत्;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	जइ (एतत् == &other)
		प्रत्यागम *एतत्;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	प्रत्यागम *एतत्;
}

तार्किक TwinePool::owns(IdString ref) ध्रुव {
	प्रत्यागम ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ध्रुव {
	जइ (ref == IdString::Null || ID::is_static(ref))
		प्रत्यागम ref;
	प्रत्यागम ref.stamped(serial_);
}

शून्य TwinePool::check_owned(IdString ref) ध्रुव {
	जइ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ध्रुव Twine_ग्रन्थिः& TwinePool::operator[](IdString ref) ध्रुव {
	check_owned(ref);
	प्रत्यागम HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	प्रति (auto &it : auto_prefixes)
		roots.निवेशय(it.second);
	प्रत्यागम HashConsPool::gc(roots);
}

ध्रुव Twine_ग्रन्थिः& TwinePool::static_ग्रन्थिः(size_t idx) { प्रत्यागम StaticTwines::ग्रन्थिः(idx); }
शून्य TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	स्थिर size_t counter = 0;
	size_t serial = ++counter;
	जइ constexpr (IdString::MAX_SERIAL == 0)
		प्रत्यागम serial;
	अण्णहा
		प्रत्यागम serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_ग्रन्थिः t) {
	जइ (free_list.empty() && STATIC_गणनम् + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_गणनम्);
	प्रत्यागम HashConsPool::add_inner(std::move(t));
}

शून्य TwinePool::dump(IdString ref, std::ostream& os) ध्रुव {
	ध्रुव Twine_ग्रन्थिः& twine = (*एतत्)[ref];
	विकल्प (twine.kind()) {
	पक्ष Twine_ग्रन्थिः::Kind::Dead:
		os << "Dead()";
		भङ्ग;
	पक्ष Twine_ग्रन्थिः::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		भङ्ग;
	पक्ष Twine_ग्रन्थिः::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		भङ्ग;
	}
	जइ (ref.isPublic())
		os << " pub";
}

शून्य TwinePool::append_str(IdString ref, std::string& out) ध्रुव {
	जइ (ref == IdString::Null)
		प्रत्यागम;
	जइ (ref.isPublic())
		out += '\\';
	ध्रुव Twine_ग्रन्थिः& twine = (*एतत्)[ref];
	विकल्प (twine.kind()) {
	पक्ष Twine_ग्रन्थिः::Kind::Dead:
		भङ्ग;
	पक्ष Twine_ग्रन्थिः::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	पक्ष Twine_ग्रन्थिः::Kind::Leaf:
		out += twine.text();
		भङ्ग;
	}
}

std::string TwinePool::str(IdString ref) ध्रुव {
	std::string out;
	append_str(ref, out);
	प्रत्यागम out;
}

std::string TwinePool::unescaped_str(IdString ref) ध्रुव {
	प्रत्यागम str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ध्रुव {
	uint32_t hash = prefix == Twine_ग्रन्थिः::NO_PREFIX ? 0 : (*एतत्)[IdString(prefix)].hash();
	प्रत्यागम Twine_ग्रन्थिः::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ध्रुव {
	प्रत्यागम find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	जइ (ref != IdString::Null)
		प्रत्यागम ref;
	प्रत्यागम add_inner(Twine_ग्रन्थिः{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ध्रुव {
	जइ (name.empty())
		प्रत्यागम IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	प्रत्यागम stamp(find_content(Twine_ग्रन्थिः::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ध्रुव {
	जइ (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		प्रत्यागम stamp(find_content(Twine_ग्रन्थिः::NO_PREFIX, t.content_str()));
	जइ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		प्रत्यागम stamp(find_content(Twine_ग्रन्थिः::NO_PREFIX, leaf->s));
	ध्रुव TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	प्रत्यागम stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	जइ (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		प्रत्यागम add(auto_prefix(ap->prefix), ap->tail);
	जइ (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		प्रत्यागम stamp(intern(Twine_ग्रन्थिः::NO_PREFIX, leaf->s));
	ध्रुव TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	प्रत्यागम add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ध्रुव std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	जइ (it != auto_prefixes.end())
		प्रत्यागम it->second;
	IdString ref = intern(Twine_ग्रन्थिः::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	प्रत्यागम ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	प्रत्यागम stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	जइ (s.empty())
		प्रत्यागम IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	प्रत्यागम stamp(intern(Twine_ग्रन्थिः::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ध्रुव TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	जइ (found != IdString::Null || ref == IdString::Null)
		प्रत्यागम found;
	ध्रुव Twine_ग्रन्थिः& t = src[ref.untag()];
	जइ (t.is_dead())
		प्रत्यागम IdString::Null;
	uint32_t prefix = Twine_ग्रन्थिः::NO_PREFIX;
	जइ (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	प्रत्यागम stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ध्रुव {
	प्रत्यागम "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ध्रुव {
	जइ (token.size() < 2 || token[0] != '#')
		प्रत्यागम IdString::Null;
	size_t sep = token.find(':');
	जइ (sep == std::string_view::npos)
		प्रत्यागम IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	प्रति (पूर्णाङ्क i = 0; i < 2; i++) {
		जइ (parts[i].empty())
			प्रत्यागम IdString::Null;
		प्रति (वर्ण c : parts[i]) {
			जइ (c < '0' || c > '9')
				प्रत्यागम IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	जइ (fields[0] != serial_)
		प्रत्यागम IdString::Null;
	IdString ref(fields[1]);
	जइ (ref == IdString::Null)
		प्रत्यागम IdString::Null;
	प्रत्यागम is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
संरचना TwineSegments {
	स्थिर constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ध्रुव TwinePool &pool, IdString ref);
	TwineSegments(ध्रुव TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	शून्य advance(size_t n);
	size_t total_size() ध्रुव;
गुप्त:
	शून्य प्रक्षिप(std::string_view seg);
	std::string_view *segs();
	ध्रुव std::string_view *segs() ध्रुव;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t गणनम् = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ध्रुव TwinePool &pool, IdString ref)
{
	जइ (ref == IdString::Null)
		प्रत्यागम;
	प्रति (IdString cur = ref.untag(); ;) {
		ध्रुव Twine_ग्रन्थिः &t = pool[cur];
		विकल्प (t.kind()) {
		पक्ष Twine_ग्रन्थिः::Kind::Suffix:
			प्रक्षिप(t.text());
			cur = t.prefix();
			अनुवर्त;
		पक्ष Twine_ग्रन्थिः::Kind::Leaf:
			प्रक्षिप(t.text());
			भङ्ग;
		पक्ष Twine_ग्रन्थिः::Kind::Dead:
			भङ्ग;
		}
		भङ्ग;
	}
	जइ (ref.isPublic())
		प्रक्षिप("\\");
	std::reverse(segs(), segs() + गणनम्);
}

TwineSegments::TwineSegments(ध्रुव TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	प्रक्षिप(tail);
}

std::string_view TwineSegments::peek()
{
	जाव (pos < गणनम्) {
		std::string_view seg = segs()[pos];
		जइ (off < seg.size())
			प्रत्यागम seg.substr(off);
		pos++;
		off = 0;
	}
	प्रत्यागम {};
}

शून्य TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ध्रुव
{
	size_t total = 0;
	प्रति (size_t i = 0; i < गणनम्; i++)
		total += segs()[i].size();
	प्रत्यागम total;
}

शून्य TwineSegments::प्रक्षिप(std::string_view seg)
{
	जइ (spill.empty() && गणनम् < SEGMENT_INLINE_DEPTH) {
		inline_segs[गणनम्++] = seg;
		प्रत्यागम;
	}
	जइ (spill.empty())
		spill.assign(inline_segs, inline_segs + गणनम्);
	spill.प्रक्षिप_back(seg);
	गणनम्++;
}

std::string_view *TwineSegments::segs() { प्रत्यागम spill.empty() ? inline_segs : spill.data(); }
ध्रुव std::string_view *TwineSegments::segs() ध्रुव { प्रत्यागम spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ध्रुव
{
	प्रत्यागम TwineSegments(*एतत्, ref).total_size();
}

तार्किक TwinePool::begins_with(IdString ref, std::string_view prefix) ध्रुव
{
	TwineSegments segs(*एतत्, ref);
	जाव (!prefix.empty()) {
		std::string_view seg = segs.peek();
		जइ (seg.empty())
			प्रत्यागम मिच्छ;
		size_t n = std::min(seg.size(), prefix.size());
		जइ (std::memcmp(seg.data(), prefix.data(), n) != 0)
			प्रत्यागम मिच्छ;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	प्रत्यागम सच्च;
}

तार्किक TwinePool::name_equal(IdString ref, std::string_view name) ध्रुव
{
	प्रत्यागम str_size(ref) == name.size() && begins_with(ref, name);
}

स्थिर पूर्णाङ्क compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	जाव (सच्च) {
		std::string_view x = sa.peek(), y = sb.peek();
		जइ (x.empty() || y.empty())
			प्रत्यागम x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		जइ (पूर्णाङ्क diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			प्रत्यागम diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ध्रुव {
	प्रत्यागम find_hashed(hash, [&](IdString ref) {
		ध्रुव Twine_ग्रन्थिः &t = (*एतत्)[ref];
		जइ (t == Twine_ग्रन्थिः::कुञ्चिका_2{prefix, text})
			प्रत्यागम सच्च;
		IdString head = prefix == Twine_ग्रन्थिः::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*एतत्, ref), sb(*एतत्, head, text);
		प्रत्यागम compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ध्रुव TwinePool& src, IdString ref) ध्रुव {
	जइ (ref == IdString::Null || ID::is_static(ref))
		प्रत्यागम ref;
	IdString idx = ref.untag();
	ध्रुव Twine_ग्रन्थिः& t = src[idx];
	जइ (t.is_dead())
		प्रत्यागम IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*एतत्, cand), sb(src, idx);
		प्रत्यागम compare_segments(sa, sb) == 0;
	});
	प्रत्यागम stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ध्रुव {
	ध्रुव Twine_ग्रन्थिः &t = (*एतत्)[ref];
	जइ (!t.is_suffix())
		प्रत्यागम IdString::Null;
	प्रत्यागम stamp(t.prefix().tag(ref.isPublic()));
}

पूर्णाङ्क TwinePool::compare_by_name(IdString a, IdString b) ध्रुव
{
	जइ (a == b)
		प्रत्यागम 0;
	जइ (a == IdString::Null)
		प्रत्यागम -1;
	जइ (b == IdString::Null)
		प्रत्यागम 1;

	जइ (a.isPublic() == b.isPublic()) {
		ध्रुव Twine_ग्रन्थिः &ta = (*एतत्)[a.untag()];
		ध्रुव Twine_ग्रन्थिः &tb = (*एतत्)[b.untag()];
		जइ (ta.is_leaf() && tb.is_leaf())
			प्रत्यागम ta.text().compare(tb.text());
		जइ (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			प्रत्यागम ta.text().compare(tb.text());
	}

	TwineSegments sa(*एतत्, a), sb(*एतत्, b);
	प्रत्यागम compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
