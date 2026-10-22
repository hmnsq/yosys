#समावेश "kernel/twine.h"
#समावेश "kernel/hashcons_impl.h"
#समावेश "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा काष्ठा HashConsPool<TwinePool, Twine_गाँठ, IdString>;

std::vector<Twine_गाँठ> StaticTwines::nodes_;

व्योम StaticTwines::init() {
	अगर (ready())
		वापस;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	क्रम (स्थिर अक्षर *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

स्थिर Twine_गाँठ &StaticTwines::गाँठ(माप_प्रकार idx) { वापस nodes_[idx]; }
bool StaticTwines::ready() { वापस nodes_.आकार() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
पूर्णांक twine_gc_गिनती;

std::string IdString::handle_token() स्थिर {
	वापस stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

माप_प्रकार IdString::handle_token_prefix(std::string_view token, bool &is_public) {
	अगर (token.substr(0, 5) == "$pub@") {
		is_public = true;
		वापस 5;
	}
	अगर (token.substr(0, 6) == "$priv@") {
		is_public = false;
		वापस 6;
	}
	वापस 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string परिणाम = ref.isPublic() ? "\\" : "";
	परिणाम += static_names[idx.raw()];
	वापस परिणाम;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	वापस static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : तथ्य(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : तथ्य(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : तथ्य(std::move(v)) {}

व्योम SmallString::store(std::string_view content)
{
	log_assert(content.आकार() <= MAX_LEN);
	release();
	len_ = content.आकार();
	अगर (len_ > INLINE_CAP) {
		ptr_ = नया अक्षर[len_];
		स_नकल(ptr_, content.तथ्य(), len_);
	} अन्यथा {
		स_नकल(inl_, content.तथ्य(), len_);
	}
}

व्योम SmallString::release()
{
	अगर (len_ > INLINE_CAP)
		मिटाओ[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(स्थिर SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	स_नकल(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::चालक=(स्थिर SmallString &other)
{
	अगर (यह != &other)
		store(other.view());
	वापस *यह;
}

SmallString &SmallString::चालक=(SmallString &&other) noexcept
{
	अगर (यह == &other)
		वापस *यह;
	release();
	स_नकल(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	वापस *यह;
}

SmallString::~SmallString() { release(); }

Twine_गाँठ::Twine_गाँठ(Twine_गाँठ &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_गाँठ &Twine_गाँठ::चालक=(Twine_गाँठ &&other) noexcept
{
	अगर (यह == &other)
		वापस *यह;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	वापस *यह;
}

std::string TwineSpec::content_str() स्थिर {
	अगर (स्वतः *leaf = std::get_if<Leaf>(&तथ्य))
		वापस leaf->s;
	स्वतः &autosfx = std::लो<AutoSuffix>(तथ्य);
	वापस *autosfx.prefix + autosfx.पुच्छ;
}

अटल std::pair<std::string_view, bool> unescape_name(std::string_view name) {
	अगर (!name.empty() && name[0] == '\\')
		वापस {name.substr(1), true};
	वापस {name, false};
}

अटल व्योम check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	क्रम (अक्षर ch : name)
		अगर ((अचिन्हित)ch <= (अचिन्हित)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(अग्र_serial()) {}
TwinePool::TwinePool(स्थिर TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(अग्र_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(अग्र_serial()) {}

TwinePool& TwinePool::चालक=(स्थिर TwinePool& other) {
	अगर (यह == &other)
		वापस *यह;
	HashConsPool::चालक=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = अग्र_serial();
	वापस *यह;
}

TwinePool& TwinePool::चालक=(TwinePool&& other) {
	अगर (यह == &other)
		वापस *यह;
	HashConsPool::चालक=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = अग्र_serial();
	वापस *यह;
}

bool TwinePool::owns(IdString ref) स्थिर {
	वापस ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) स्थिर {
	अगर (ref == IdString::Null || ID::is_static(ref))
		वापस ref;
	वापस ref.stamped(serial_);
}

व्योम TwinePool::check_owned(IdString ref) स्थिर {
	अगर constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

स्थिर Twine_गाँठ& TwinePool::चालक[](IdString ref) स्थिर {
	check_owned(ref);
	वापस HashConsPool::चालक[](ref);
}

माप_प्रकार TwinePool::gc(pool<IdString>& roots) {
	क्रम (स्वतः &it : auto_prefixes)
		roots.जोड़ो(it.second);
	वापस HashConsPool::gc(roots);
}

स्थिर Twine_गाँठ& TwinePool::static_गाँठ(माप_प्रकार idx) { वापस StaticTwines::गाँठ(idx); }
व्योम TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

माप_प्रकार TwinePool::अग्र_serial() {
	अटल माप_प्रकार गणक = 0;
	माप_प्रकार serial = ++गणक;
	अगर constexpr (IdString::MAX_SERIAL == 0)
		वापस serial;
	अन्यथा
		वापस serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_गाँठ t) {
	अगर (free_list.empty() && STATIC_गिनती + backing.आकार() > IdString::MAX_सूचकांक)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_सूचकांक - STATIC_गिनती);
	वापस HashConsPool::add_inner(std::move(t));
}

व्योम TwinePool::dump(IdString ref, std::ब_प्रवाह& os) स्थिर {
	स्थिर Twine_गाँठ& twine = (*यह)[ref];
	चयन (twine.kind()) {
	हाल Twine_गाँठ::Kind::Dead:
		os << "Dead()";
		अवरोध;
	हाल Twine_गाँठ::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		अवरोध;
	हाल Twine_गाँठ::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		अवरोध;
	}
	अगर (ref.isPublic())
		os << " pub";
}

व्योम TwinePool::append_str(IdString ref, std::string& out) स्थिर {
	अगर (ref == IdString::Null)
		वापस;
	अगर (ref.isPublic())
		out += '\\';
	स्थिर Twine_गाँठ& twine = (*यह)[ref];
	चयन (twine.kind()) {
	हाल Twine_गाँठ::Kind::Dead:
		अवरोध;
	हाल Twine_गाँठ::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	हाल Twine_गाँठ::Kind::Leaf:
		out += twine.text();
		अवरोध;
	}
}

std::string TwinePool::str(IdString ref) स्थिर {
	std::string out;
	append_str(ref, out);
	वापस out;
}

std::string TwinePool::unescaped_str(IdString ref) स्थिर {
	वापस str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) स्थिर {
	uint32_t hash = prefix == Twine_गाँठ::NO_PREFIX ? 0 : (*यह)[IdString(prefix)].hash();
	वापस Twine_गाँठ::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) स्थिर {
	वापस find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	अगर (ref != IdString::Null)
		वापस ref;
	वापस add_inner(Twine_गाँठ{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) स्थिर {
	अगर (name.empty())
		वापस IdString::Null;
	check_name(name);
	स्वतः [content, is_public] = unescape_name(name);
	वापस stamp(find_content(Twine_गाँठ::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) स्थिर {
	अगर (std::holds_alternative<TwineSpec::AutoSuffix>(t.तथ्य))
		वापस stamp(find_content(Twine_गाँठ::NO_PREFIX, t.content_str()));
	अगर (स्वतः *leaf = std::get_if<TwineSpec::Leaf>(&t.तथ्य))
		वापस stamp(find_content(Twine_गाँठ::NO_PREFIX, leaf->s));
	स्थिर TwineSpec::Suffix &sfx = std::लो<TwineSpec::Suffix>(t.तथ्य);
	वापस stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.पुच्छ).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	अगर (स्वतः *ap = std::get_if<TwineSpec::AutoSuffix>(&t.तथ्य))
		वापस add(auto_prefix(ap->prefix), ap->पुच्छ);
	अगर (स्वतः *leaf = std::get_if<TwineSpec::Leaf>(&t.तथ्य))
		वापस stamp(intern(Twine_गाँठ::NO_PREFIX, leaf->s));
	स्थिर TwineSpec::Suffix &sfx = std::लो<TwineSpec::Suffix>(t.तथ्य);
	वापस add(sfx.prefix, sfx.पुच्छ);
}

IdString TwinePool::auto_prefix(स्थिर std::string *prefix) {
	स्वतः it = auto_prefixes.find(prefix);
	अगर (it != auto_prefixes.end())
		वापस it->second;
	IdString ref = intern(Twine_गाँठ::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	वापस ref;
}

IdString TwinePool::add(IdString prefix, std::string_view पुच्छ) {
	वापस stamp(intern((uint32_t)prefix.untag().raw(), पुच्छ).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	अगर (s.empty())
		वापस IdString::Null;
	check_name(s);
	स्वतः [content, is_public] = unescape_name(s);
	वापस stamp(intern(Twine_गाँठ::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(स्थिर TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	अगर (found != IdString::Null || ref == IdString::Null)
		वापस found;
	स्थिर Twine_गाँठ& t = src[ref.untag()];
	अगर (t.is_dead())
		वापस IdString::Null;
	uint32_t prefix = Twine_गाँठ::NO_PREFIX;
	अगर (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	वापस stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) स्थिर {
	वापस "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) स्थिर {
	अगर (token.आकार() < 2 || token[0] != '#')
		वापस IdString::Null;
	माप_प्रकार sep = token.find(':');
	अगर (sep == std::string_view::npos)
		वापस IdString::Null;
	माप_प्रकार fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	क्रम (पूर्णांक i = 0; i < 2; i++) {
		अगर (parts[i].empty())
			वापस IdString::Null;
		क्रम (अक्षर c : parts[i]) {
			अगर (c < '0' || c > '9')
				वापस IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	अगर (fields[0] != serial_)
		वापस IdString::Null;
	IdString ref(fields[1]);
	अगर (ref == IdString::Null)
		वापस IdString::Null;
	वापस is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
काष्ठा TwineSegments {
	अटल constexpr माप_प्रकार SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(स्थिर TwinePool &pool, IdString ref);
	TwineSegments(स्थिर TwinePool &pool, IdString शीर्ष_2, std::string_view पुच्छ);
	std::string_view peek();
	व्योम advance(माप_प्रकार n);
	माप_प्रकार योग_आकार() स्थिर;
निजी:
	व्योम धकेलो(std::string_view seg);
	std::string_view *segs();
	स्थिर std::string_view *segs() स्थिर;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	माप_प्रकार गिनती = 0;
	माप_प्रकार pos = 0;
	माप_प्रकार off = 0;
};

TwineSegments::TwineSegments(स्थिर TwinePool &pool, IdString ref)
{
	अगर (ref == IdString::Null)
		वापस;
	क्रम (IdString cur = ref.untag(); ;) {
		स्थिर Twine_गाँठ &t = pool[cur];
		चयन (t.kind()) {
		हाल Twine_गाँठ::Kind::Suffix:
			धकेलो(t.text());
			cur = t.prefix();
			जारी;
		हाल Twine_गाँठ::Kind::Leaf:
			धकेलो(t.text());
			अवरोध;
		हाल Twine_गाँठ::Kind::Dead:
			अवरोध;
		}
		अवरोध;
	}
	अगर (ref.isPublic())
		धकेलो("\\");
	std::reverse(segs(), segs() + गिनती);
}

TwineSegments::TwineSegments(स्थिर TwinePool &pool, IdString शीर्ष_2, std::string_view पुच्छ)
	: TwineSegments(pool, शीर्ष_2)
{
	धकेलो(पुच्छ);
}

std::string_view TwineSegments::peek()
{
	जबतक (pos < गिनती) {
		std::string_view seg = segs()[pos];
		अगर (off < seg.आकार())
			वापस seg.substr(off);
		pos++;
		off = 0;
	}
	वापस {};
}

व्योम TwineSegments::advance(माप_प्रकार n) { off += n; }

माप_प्रकार TwineSegments::योग_आकार() स्थिर
{
	माप_प्रकार योग_2 = 0;
	क्रम (माप_प्रकार i = 0; i < गिनती; i++)
		योग_2 += segs()[i].आकार();
	वापस योग_2;
}

व्योम TwineSegments::धकेलो(std::string_view seg)
{
	अगर (spill.empty() && गिनती < SEGMENT_INLINE_DEPTH) {
		inline_segs[गिनती++] = seg;
		वापस;
	}
	अगर (spill.empty())
		spill.assign(inline_segs, inline_segs + गिनती);
	spill.धकेलो_back(seg);
	गिनती++;
}

std::string_view *TwineSegments::segs() { वापस spill.empty() ? inline_segs : spill.तथ्य(); }
स्थिर std::string_view *TwineSegments::segs() स्थिर { वापस spill.empty() ? inline_segs : spill.तथ्य(); }

माप_प्रकार TwinePool::str_आकार(IdString ref) स्थिर
{
	वापस TwineSegments(*यह, ref).योग_आकार();
}

bool TwinePool::begins_with(IdString ref, std::string_view prefix) स्थिर
{
	TwineSegments segs(*यह, ref);
	जबतक (!prefix.empty()) {
		std::string_view seg = segs.peek();
		अगर (seg.empty())
			वापस false;
		माप_प्रकार n = std::min(seg.आकार(), prefix.आकार());
		अगर (std::स_भेद(seg.तथ्य(), prefix.तथ्य(), n) != 0)
			वापस false;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	वापस true;
}

bool TwinePool::name_equal(IdString ref, std::string_view name) स्थिर
{
	वापस str_आकार(ref) == name.आकार() && begins_with(ref, name);
}

अटल पूर्णांक compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	जबतक (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		अगर (x.empty() || y.empty())
			वापस x.empty() ? (y.empty() ? 0 : -1) : 1;
		माप_प्रकार n = std::min(x.आकार(), y.आकार());
		अगर (पूर्णांक diff = std::स_भेद(x.तथ्य(), y.तथ्य(), n); diff != 0)
			वापस diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) स्थिर {
	वापस find_hashed(hash, [&](IdString ref) {
		स्थिर Twine_गाँठ &t = (*यह)[ref];
		अगर (t == Twine_गाँठ::कुंजी_2{prefix, text})
			वापस true;
		IdString शीर्ष_2 = prefix == Twine_गाँठ::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*यह, ref), sb(*यह, शीर्ष_2, text);
		वापस compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(स्थिर TwinePool& src, IdString ref) स्थिर {
	अगर (ref == IdString::Null || ID::is_static(ref))
		वापस ref;
	IdString idx = ref.untag();
	स्थिर Twine_गाँठ& t = src[idx];
	अगर (t.is_dead())
		वापस IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*यह, cand), sb(src, idx);
		वापस compare_segments(sa, sb) == 0;
	});
	वापस stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) स्थिर {
	स्थिर Twine_गाँठ &t = (*यह)[ref];
	अगर (!t.is_suffix())
		वापस IdString::Null;
	वापस stamp(t.prefix().tag(ref.isPublic()));
}

पूर्णांक TwinePool::compare_by_name(IdString a, IdString b) स्थिर
{
	अगर (a == b)
		वापस 0;
	अगर (a == IdString::Null)
		वापस -1;
	अगर (b == IdString::Null)
		वापस 1;

	अगर (a.isPublic() == b.isPublic()) {
		स्थिर Twine_गाँठ &ta = (*यह)[a.untag()];
		स्थिर Twine_गाँठ &tb = (*यह)[b.untag()];
		अगर (ta.is_leaf() && tb.is_leaf())
			वापस ta.text().compare(tb.text());
		अगर (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			वापस ta.text().compare(tb.text());
	}

	TwineSegments sa(*यह, a), sb(*यह, b);
	वापस compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
