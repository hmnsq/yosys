#अन्तर्भाव "kernel/twine.h"
#अन्तर्भाव "kernel/hashcons_impl.h"
#अन्तर्भाव "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा संरचना HashConsPool<TwinePool, Twine_गाँठो, IdString>;

std::vector<Twine_गाँठो> StaticTwines::nodes_;

शून्य StaticTwines::init() {
	यदि (ready())
		फर्काऊ;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	लागि (अचर वर्ण *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

अचर Twine_गाँठो &StaticTwines::गाँठो(माप_प्रकार idx) { फर्काऊ nodes_[idx]; }
तार्किक StaticTwines::ready() { फर्काऊ nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
पूर्णाङ्क twine_gc_गन्ती;

std::string IdString::handle_token() अचर {
	फर्काऊ stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

माप_प्रकार IdString::handle_token_prefix(std::string_view token, तार्किक &is_public) {
	यदि (token.substr(0, 5) == "$pub@") {
		is_public = सत्य;
		फर्काऊ 5;
	}
	यदि (token.substr(0, 6) == "$priv@") {
		is_public = असत्य;
		फर्काऊ 6;
	}
	फर्काऊ 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	फर्काऊ result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	फर्काऊ static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

शून्य SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	यदि (len_ > INLINE_CAP) {
		ptr_ = नूतन वर्ण[len_];
		स_नकल(ptr_, content.data(), len_);
	} अन्यथा {
		स_नकल(inl_, content.data(), len_);
	}
}

शून्य SmallString::release()
{
	यदि (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(अचर SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	स_नकल(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::चालक=(अचर SmallString &other)
{
	यदि (एतत् != &other)
		store(other.view());
	फर्काऊ *एतत्;
}

SmallString &SmallString::चालक=(SmallString &&other) noexcept
{
	यदि (एतत् == &other)
		फर्काऊ *एतत्;
	release();
	स_नकल(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	फर्काऊ *एतत्;
}

SmallString::~SmallString() { release(); }

Twine_गाँठो::Twine_गाँठो(Twine_गाँठो &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_गाँठो &Twine_गाँठो::चालक=(Twine_गाँठो &&other) noexcept
{
	यदि (एतत् == &other)
		फर्काऊ *एतत्;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	फर्काऊ *एतत्;
}

std::string TwineSpec::content_str() अचर {
	यदि (स्वतः *leaf = std::get_if<Leaf>(&data))
		फर्काऊ leaf->s;
	स्वतः &autosfx = std::get<AutoSuffix>(data);
	फर्काऊ *autosfx.prefix + autosfx.tail;
}

स्थिर std::pair<std::string_view, तार्किक> unescape_name(std::string_view name) {
	यदि (!name.empty() && name[0] == '\\')
		फर्काऊ {name.substr(1), सत्य};
	फर्काऊ {name, असत्य};
}

स्थिर शून्य check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	लागि (वर्ण ch : name)
		यदि ((अचिन्हित)ch <= (अचिन्हित)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(अचर TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::चालक=(अचर TwinePool& other) {
	यदि (एतत् == &other)
		फर्काऊ *एतत्;
	HashConsPool::चालक=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	फर्काऊ *एतत्;
}

TwinePool& TwinePool::चालक=(TwinePool&& other) {
	यदि (एतत् == &other)
		फर्काऊ *एतत्;
	HashConsPool::चालक=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	फर्काऊ *एतत्;
}

तार्किक TwinePool::owns(IdString ref) अचर {
	फर्काऊ ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) अचर {
	यदि (ref == IdString::Null || ID::is_static(ref))
		फर्काऊ ref;
	फर्काऊ ref.stamped(serial_);
}

शून्य TwinePool::check_owned(IdString ref) अचर {
	यदि constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

अचर Twine_गाँठो& TwinePool::चालक[](IdString ref) अचर {
	check_owned(ref);
	फर्काऊ HashConsPool::चालक[](ref);
}

माप_प्रकार TwinePool::gc(pool<IdString>& roots) {
	लागि (स्वतः &it : auto_prefixes)
		roots.घुसाउ(it.second);
	फर्काऊ HashConsPool::gc(roots);
}

अचर Twine_गाँठो& TwinePool::static_गाँठो(माप_प्रकार idx) { फर्काऊ StaticTwines::गाँठो(idx); }
शून्य TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

माप_प्रकार TwinePool::next_serial() {
	स्थिर माप_प्रकार counter = 0;
	माप_प्रकार serial = ++counter;
	यदि constexpr (IdString::MAX_SERIAL == 0)
		फर्काऊ serial;
	अन्यथा
		फर्काऊ serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_गाँठो t) {
	यदि (free_list.empty() && STATIC_गन्ती + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_गन्ती);
	फर्काऊ HashConsPool::add_inner(std::move(t));
}

शून्य TwinePool::dump(IdString ref, std::ostream& os) अचर {
	अचर Twine_गाँठो& twine = (*एतत्)[ref];
	विकल्प (twine.kind()) {
	पक्ष Twine_गाँठो::Kind::Dead:
		os << "Dead()";
		रोक;
	पक्ष Twine_गाँठो::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		रोक;
	पक्ष Twine_गाँठो::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		रोक;
	}
	यदि (ref.isPublic())
		os << " pub";
}

शून्य TwinePool::append_str(IdString ref, std::string& out) अचर {
	यदि (ref == IdString::Null)
		फर्काऊ;
	यदि (ref.isPublic())
		out += '\\';
	अचर Twine_गाँठो& twine = (*एतत्)[ref];
	विकल्प (twine.kind()) {
	पक्ष Twine_गाँठो::Kind::Dead:
		रोक;
	पक्ष Twine_गाँठो::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	पक्ष Twine_गाँठो::Kind::Leaf:
		out += twine.text();
		रोक;
	}
}

std::string TwinePool::str(IdString ref) अचर {
	std::string out;
	append_str(ref, out);
	फर्काऊ out;
}

std::string TwinePool::unescaped_str(IdString ref) अचर {
	फर्काऊ str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) अचर {
	uint32_t hash = prefix == Twine_गाँठो::NO_PREFIX ? 0 : (*एतत्)[IdString(prefix)].hash();
	फर्काऊ Twine_गाँठो::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) अचर {
	फर्काऊ find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	यदि (ref != IdString::Null)
		फर्काऊ ref;
	फर्काऊ add_inner(Twine_गाँठो{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) अचर {
	यदि (name.empty())
		फर्काऊ IdString::Null;
	check_name(name);
	स्वतः [content, is_public] = unescape_name(name);
	फर्काऊ stamp(find_content(Twine_गाँठो::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) अचर {
	यदि (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		फर्काऊ stamp(find_content(Twine_गाँठो::NO_PREFIX, t.content_str()));
	यदि (स्वतः *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		फर्काऊ stamp(find_content(Twine_गाँठो::NO_PREFIX, leaf->s));
	अचर TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	फर्काऊ stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	यदि (स्वतः *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		फर्काऊ add(auto_prefix(ap->prefix), ap->tail);
	यदि (स्वतः *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		फर्काऊ stamp(intern(Twine_गाँठो::NO_PREFIX, leaf->s));
	अचर TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	फर्काऊ add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(अचर std::string *prefix) {
	स्वतः it = auto_prefixes.find(prefix);
	यदि (it != auto_prefixes.end())
		फर्काऊ it->second;
	IdString ref = intern(Twine_गाँठो::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	फर्काऊ ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	फर्काऊ stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	यदि (s.empty())
		फर्काऊ IdString::Null;
	check_name(s);
	स्वतः [content, is_public] = unescape_name(s);
	फर्काऊ stamp(intern(Twine_गाँठो::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(अचर TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	यदि (found != IdString::Null || ref == IdString::Null)
		फर्काऊ found;
	अचर Twine_गाँठो& t = src[ref.untag()];
	यदि (t.is_dead())
		फर्काऊ IdString::Null;
	uint32_t prefix = Twine_गाँठो::NO_PREFIX;
	यदि (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	फर्काऊ stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) अचर {
	फर्काऊ "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) अचर {
	यदि (token.size() < 2 || token[0] != '#')
		फर्काऊ IdString::Null;
	माप_प्रकार sep = token.find(':');
	यदि (sep == std::string_view::npos)
		फर्काऊ IdString::Null;
	माप_प्रकार fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	लागि (पूर्णाङ्क i = 0; i < 2; i++) {
		यदि (parts[i].empty())
			फर्काऊ IdString::Null;
		लागि (वर्ण c : parts[i]) {
			यदि (c < '0' || c > '9')
				फर्काऊ IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	यदि (fields[0] != serial_)
		फर्काऊ IdString::Null;
	IdString ref(fields[1]);
	यदि (ref == IdString::Null)
		फर्काऊ IdString::Null;
	फर्काऊ is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
संरचना TwineSegments {
	स्थिर constexpr माप_प्रकार SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(अचर TwinePool &pool, IdString ref);
	TwineSegments(अचर TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	शून्य advance(माप_प्रकार n);
	माप_प्रकार total_size() अचर;
गुप्त:
	शून्य थप(std::string_view seg);
	std::string_view *segs();
	अचर std::string_view *segs() अचर;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	माप_प्रकार गन्ती = 0;
	माप_प्रकार pos = 0;
	माप_प्रकार off = 0;
};

TwineSegments::TwineSegments(अचर TwinePool &pool, IdString ref)
{
	यदि (ref == IdString::Null)
		फर्काऊ;
	लागि (IdString cur = ref.untag(); ;) {
		अचर Twine_गाँठो &t = pool[cur];
		विकल्प (t.kind()) {
		पक्ष Twine_गाँठो::Kind::Suffix:
			थप(t.text());
			cur = t.prefix();
			जारी;
		पक्ष Twine_गाँठो::Kind::Leaf:
			थप(t.text());
			रोक;
		पक्ष Twine_गाँठो::Kind::Dead:
			रोक;
		}
		रोक;
	}
	यदि (ref.isPublic())
		थप("\\");
	std::reverse(segs(), segs() + गन्ती);
}

TwineSegments::TwineSegments(अचर TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	थप(tail);
}

std::string_view TwineSegments::peek()
{
	जबसम्म (pos < गन्ती) {
		std::string_view seg = segs()[pos];
		यदि (off < seg.size())
			फर्काऊ seg.substr(off);
		pos++;
		off = 0;
	}
	फर्काऊ {};
}

शून्य TwineSegments::advance(माप_प्रकार n) { off += n; }

माप_प्रकार TwineSegments::total_size() अचर
{
	माप_प्रकार total = 0;
	लागि (माप_प्रकार i = 0; i < गन्ती; i++)
		total += segs()[i].size();
	फर्काऊ total;
}

शून्य TwineSegments::थप(std::string_view seg)
{
	यदि (spill.empty() && गन्ती < SEGMENT_INLINE_DEPTH) {
		inline_segs[गन्ती++] = seg;
		फर्काऊ;
	}
	यदि (spill.empty())
		spill.assign(inline_segs, inline_segs + गन्ती);
	spill.थप_back(seg);
	गन्ती++;
}

std::string_view *TwineSegments::segs() { फर्काऊ spill.empty() ? inline_segs : spill.data(); }
अचर std::string_view *TwineSegments::segs() अचर { फर्काऊ spill.empty() ? inline_segs : spill.data(); }

माप_प्रकार TwinePool::str_size(IdString ref) अचर
{
	फर्काऊ TwineSegments(*एतत्, ref).total_size();
}

तार्किक TwinePool::begins_with(IdString ref, std::string_view prefix) अचर
{
	TwineSegments segs(*एतत्, ref);
	जबसम्म (!prefix.empty()) {
		std::string_view seg = segs.peek();
		यदि (seg.empty())
			फर्काऊ असत्य;
		माप_प्रकार n = std::min(seg.size(), prefix.size());
		यदि (std::स_भेद(seg.data(), prefix.data(), n) != 0)
			फर्काऊ असत्य;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	फर्काऊ सत्य;
}

तार्किक TwinePool::name_equal(IdString ref, std::string_view name) अचर
{
	फर्काऊ str_size(ref) == name.size() && begins_with(ref, name);
}

स्थिर पूर्णाङ्क compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	जबसम्म (सत्य) {
		std::string_view x = sa.peek(), y = sb.peek();
		यदि (x.empty() || y.empty())
			फर्काऊ x.empty() ? (y.empty() ? 0 : -1) : 1;
		माप_प्रकार n = std::min(x.size(), y.size());
		यदि (पूर्णाङ्क diff = std::स_भेद(x.data(), y.data(), n); diff != 0)
			फर्काऊ diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) अचर {
	फर्काऊ find_hashed(hash, [&](IdString ref) {
		अचर Twine_गाँठो &t = (*एतत्)[ref];
		यदि (t == Twine_गाँठो::साँचो_2{prefix, text})
			फर्काऊ सत्य;
		IdString head = prefix == Twine_गाँठो::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*एतत्, ref), sb(*एतत्, head, text);
		फर्काऊ compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(अचर TwinePool& src, IdString ref) अचर {
	यदि (ref == IdString::Null || ID::is_static(ref))
		फर्काऊ ref;
	IdString idx = ref.untag();
	अचर Twine_गाँठो& t = src[idx];
	यदि (t.is_dead())
		फर्काऊ IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*एतत्, cand), sb(src, idx);
		फर्काऊ compare_segments(sa, sb) == 0;
	});
	फर्काऊ stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) अचर {
	अचर Twine_गाँठो &t = (*एतत्)[ref];
	यदि (!t.is_suffix())
		फर्काऊ IdString::Null;
	फर्काऊ stamp(t.prefix().tag(ref.isPublic()));
}

पूर्णाङ्क TwinePool::compare_by_name(IdString a, IdString b) अचर
{
	यदि (a == b)
		फर्काऊ 0;
	यदि (a == IdString::Null)
		फर्काऊ -1;
	यदि (b == IdString::Null)
		फर्काऊ 1;

	यदि (a.isPublic() == b.isPublic()) {
		अचर Twine_गाँठो &ta = (*एतत्)[a.untag()];
		अचर Twine_गाँठो &tb = (*एतत्)[b.untag()];
		यदि (ta.is_leaf() && tb.is_leaf())
			फर्काऊ ta.text().compare(tb.text());
		यदि (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			फर्काऊ ta.text().compare(tb.text());
	}

	TwineSegments sa(*एतत्, a), sb(*एतत्, b);
	फर्काऊ compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
