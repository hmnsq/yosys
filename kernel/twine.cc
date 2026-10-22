#अन्तर्भाव "kernel/twine.h"
#अन्तर्भाव "kernel/hashcons_impl.h"
#अन्तर्भाव "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template संरचना HashConsPool<TwinePool, Twine_ग्रन्थिः, IdString>;

std::vector<Twine_ग्रन्थिः> StaticTwines::nodes_;

शून्य StaticTwines::init() {
	सचे (ready())
		पटिनिवत्त;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	अत्थाय (ध्रुव वर्ण *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ध्रुव Twine_ग्रन्थिः &StaticTwines::ग्रन्थिः(size_t idx) { पटिनिवत्त nodes_[idx]; }
तार्किक StaticTwines::ready() { पटिनिवत्त nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
पूर्णाङ्क twine_gc_गणनम्;

std::string IdString::handle_token() ध्रुव {
	पटिनिवत्त stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, तार्किक &is_public) {
	सचे (token.substr(0, 5) == "$pub@") {
		is_public = सच्च;
		पटिनिवत्त 5;
	}
	सचे (token.substr(0, 6) == "$priv@") {
		is_public = मिच्छा;
		पटिनिवत्त 6;
	}
	पटिनिवत्त 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	पटिनिवत्त result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	पटिनिवत्त static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

शून्य SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	सचे (len_ > INLINE_CAP) {
		ptr_ = नूतन वर्ण[len_];
		memcpy(ptr_, content.data(), len_);
	} अञ्ञथा {
		memcpy(inl_, content.data(), len_);
	}
}

शून्य SmallString::release()
{
	सचे (len_ > INLINE_CAP)
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
	सचे (एतत् != &other)
		store(other.view());
	पटिनिवत्त *एतत्;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	सचे (एतत् == &other)
		पटिनिवत्त *एतत्;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	पटिनिवत्त *एतत्;
}

SmallString::~SmallString() { release(); }

Twine_ग्रन्थिः::Twine_ग्रन्थिः(Twine_ग्रन्थिः &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_ग्रन्थिः &Twine_ग्रन्थिः::operator=(Twine_ग्रन्थिः &&other) noexcept
{
	सचे (एतत् == &other)
		पटिनिवत्त *एतत्;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	पटिनिवत्त *एतत्;
}

std::string TwineSpec::content_str() ध्रुव {
	सचे (auto *leaf = std::get_if<Leaf>(&data))
		पटिनिवत्त leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	पटिनिवत्त *autosfx.prefix + autosfx.tail;
}

स्थिर std::pair<std::string_view, तार्किक> unescape_name(std::string_view name) {
	सचे (!name.empty() && name[0] == '\\')
		पटिनिवत्त {name.substr(1), सच्च};
	पटिनिवत्त {name, मिच्छा};
}

स्थिर शून्य check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	अत्थाय (वर्ण ch : name)
		सचे ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ध्रुव TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ध्रुव TwinePool& other) {
	सचे (एतत् == &other)
		पटिनिवत्त *एतत्;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	पटिनिवत्त *एतत्;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	सचे (एतत् == &other)
		पटिनिवत्त *एतत्;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	पटिनिवत्त *एतत्;
}

तार्किक TwinePool::owns(IdString ref) ध्रुव {
	पटिनिवत्त ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ध्रुव {
	सचे (ref == IdString::Null || ID::is_static(ref))
		पटिनिवत्त ref;
	पटिनिवत्त ref.stamped(serial_);
}

शून्य TwinePool::check_owned(IdString ref) ध्रुव {
	सचे constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ध्रुव Twine_ग्रन्थिः& TwinePool::operator[](IdString ref) ध्रुव {
	check_owned(ref);
	पटिनिवत्त HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	अत्थाय (auto &it : auto_prefixes)
		roots.निवेशय(it.second);
	पटिनिवत्त HashConsPool::gc(roots);
}

ध्रुव Twine_ग्रन्थिः& TwinePool::static_ग्रन्थिः(size_t idx) { पटिनिवत्त StaticTwines::ग्रन्थिः(idx); }
शून्य TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	स्थिर size_t counter = 0;
	size_t serial = ++counter;
	सचे constexpr (IdString::MAX_SERIAL == 0)
		पटिनिवत्त serial;
	अञ्ञथा
		पटिनिवत्त serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_ग्रन्थिः t) {
	सचे (free_list.empty() && STATIC_गणनम् + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_गणनम्);
	पटिनिवत्त HashConsPool::add_inner(std::move(t));
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
	सचे (ref.isPublic())
		os << " pub";
}

शून्य TwinePool::append_str(IdString ref, std::string& out) ध्रुव {
	सचे (ref == IdString::Null)
		पटिनिवत्त;
	सचे (ref.isPublic())
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
	पटिनिवत्त out;
}

std::string TwinePool::unescaped_str(IdString ref) ध्रुव {
	पटिनिवत्त str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ध्रुव {
	uint32_t hash = prefix == Twine_ग्रन्थिः::NO_PREFIX ? 0 : (*एतत्)[IdString(prefix)].hash();
	पटिनिवत्त Twine_ग्रन्थिः::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ध्रुव {
	पटिनिवत्त find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	सचे (ref != IdString::Null)
		पटिनिवत्त ref;
	पटिनिवत्त add_inner(Twine_ग्रन्थिः{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ध्रुव {
	सचे (name.empty())
		पटिनिवत्त IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	पटिनिवत्त stamp(find_content(Twine_ग्रन्थिः::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ध्रुव {
	सचे (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		पटिनिवत्त stamp(find_content(Twine_ग्रन्थिः::NO_PREFIX, t.content_str()));
	सचे (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		पटिनिवत्त stamp(find_content(Twine_ग्रन्थिः::NO_PREFIX, leaf->s));
	ध्रुव TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	पटिनिवत्त stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	सचे (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		पटिनिवत्त add(auto_prefix(ap->prefix), ap->tail);
	सचे (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		पटिनिवत्त stamp(intern(Twine_ग्रन्थिः::NO_PREFIX, leaf->s));
	ध्रुव TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	पटिनिवत्त add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ध्रुव std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	सचे (it != auto_prefixes.end())
		पटिनिवत्त it->second;
	IdString ref = intern(Twine_ग्रन्थिः::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	पटिनिवत्त ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	पटिनिवत्त stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	सचे (s.empty())
		पटिनिवत्त IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	पटिनिवत्त stamp(intern(Twine_ग्रन्थिः::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ध्रुव TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	सचे (found != IdString::Null || ref == IdString::Null)
		पटिनिवत्त found;
	ध्रुव Twine_ग्रन्थिः& t = src[ref.untag()];
	सचे (t.is_dead())
		पटिनिवत्त IdString::Null;
	uint32_t prefix = Twine_ग्रन्थिः::NO_PREFIX;
	सचे (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	पटिनिवत्त stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ध्रुव {
	पटिनिवत्त "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ध्रुव {
	सचे (token.size() < 2 || token[0] != '#')
		पटिनिवत्त IdString::Null;
	size_t sep = token.find(':');
	सचे (sep == std::string_view::npos)
		पटिनिवत्त IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	अत्थाय (पूर्णाङ्क i = 0; i < 2; i++) {
		सचे (parts[i].empty())
			पटिनिवत्त IdString::Null;
		अत्थाय (वर्ण c : parts[i]) {
			सचे (c < '0' || c > '9')
				पटिनिवत्त IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	सचे (fields[0] != serial_)
		पटिनिवत्त IdString::Null;
	IdString ref(fields[1]);
	सचे (ref == IdString::Null)
		पटिनिवत्त IdString::Null;
	पटिनिवत्त is_live(ref) ? ref : IdString::Null;
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
	सचे (ref == IdString::Null)
		पटिनिवत्त;
	अत्थाय (IdString cur = ref.untag(); ;) {
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
	सचे (ref.isPublic())
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
	याव (pos < गणनम्) {
		std::string_view seg = segs()[pos];
		सचे (off < seg.size())
			पटिनिवत्त seg.substr(off);
		pos++;
		off = 0;
	}
	पटिनिवत्त {};
}

शून्य TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ध्रुव
{
	size_t total = 0;
	अत्थाय (size_t i = 0; i < गणनम्; i++)
		total += segs()[i].size();
	पटिनिवत्त total;
}

शून्य TwineSegments::प्रक्षिप(std::string_view seg)
{
	सचे (spill.empty() && गणनम् < SEGMENT_INLINE_DEPTH) {
		inline_segs[गणनम्++] = seg;
		पटिनिवत्त;
	}
	सचे (spill.empty())
		spill.assign(inline_segs, inline_segs + गणनम्);
	spill.प्रक्षिप_back(seg);
	गणनम्++;
}

std::string_view *TwineSegments::segs() { पटिनिवत्त spill.empty() ? inline_segs : spill.data(); }
ध्रुव std::string_view *TwineSegments::segs() ध्रुव { पटिनिवत्त spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ध्रुव
{
	पटिनिवत्त TwineSegments(*एतत्, ref).total_size();
}

तार्किक TwinePool::begins_with(IdString ref, std::string_view prefix) ध्रुव
{
	TwineSegments segs(*एतत्, ref);
	याव (!prefix.empty()) {
		std::string_view seg = segs.peek();
		सचे (seg.empty())
			पटिनिवत्त मिच्छा;
		size_t n = std::min(seg.size(), prefix.size());
		सचे (std::memcmp(seg.data(), prefix.data(), n) != 0)
			पटिनिवत्त मिच्छा;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	पटिनिवत्त सच्च;
}

तार्किक TwinePool::name_equal(IdString ref, std::string_view name) ध्रुव
{
	पटिनिवत्त str_size(ref) == name.size() && begins_with(ref, name);
}

स्थिर पूर्णाङ्क compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	याव (सच्च) {
		std::string_view x = sa.peek(), y = sb.peek();
		सचे (x.empty() || y.empty())
			पटिनिवत्त x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		सचे (पूर्णाङ्क diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			पटिनिवत्त diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ध्रुव {
	पटिनिवत्त find_hashed(hash, [&](IdString ref) {
		ध्रुव Twine_ग्रन्थिः &t = (*एतत्)[ref];
		सचे (t == Twine_ग्रन्थिः::कुञ्चिका_2{prefix, text})
			पटिनिवत्त सच्च;
		IdString head = prefix == Twine_ग्रन्थिः::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*एतत्, ref), sb(*एतत्, head, text);
		पटिनिवत्त compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ध्रुव TwinePool& src, IdString ref) ध्रुव {
	सचे (ref == IdString::Null || ID::is_static(ref))
		पटिनिवत्त ref;
	IdString idx = ref.untag();
	ध्रुव Twine_ग्रन्थिः& t = src[idx];
	सचे (t.is_dead())
		पटिनिवत्त IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*एतत्, cand), sb(src, idx);
		पटिनिवत्त compare_segments(sa, sb) == 0;
	});
	पटिनिवत्त stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ध्रुव {
	ध्रुव Twine_ग्रन्थिः &t = (*एतत्)[ref];
	सचे (!t.is_suffix())
		पटिनिवत्त IdString::Null;
	पटिनिवत्त stamp(t.prefix().tag(ref.isPublic()));
}

पूर्णाङ्क TwinePool::compare_by_name(IdString a, IdString b) ध्रुव
{
	सचे (a == b)
		पटिनिवत्त 0;
	सचे (a == IdString::Null)
		पटिनिवत्त -1;
	सचे (b == IdString::Null)
		पटिनिवत्त 1;

	सचे (a.isPublic() == b.isPublic()) {
		ध्रुव Twine_ग्रन्थिः &ta = (*एतत्)[a.untag()];
		ध्रुव Twine_ग्रन्थिः &tb = (*एतत्)[b.untag()];
		सचे (ta.is_leaf() && tb.is_leaf())
			पटिनिवत्त ta.text().compare(tb.text());
		सचे (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			पटिनिवत्त ta.text().compare(tb.text());
	}

	TwineSegments sa(*एतत्, a), sb(*एतत्, b);
	पटिनिवत्त compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
