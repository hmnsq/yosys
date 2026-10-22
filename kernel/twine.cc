#अन्तर्भाव "kernel/twine.h"
#अन्तर्भाव "kernel/hashcons_impl.h"
#अन्तर्भाव "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा संरचना HashConsPool<TwinePool, Twine_गाठ, IdString>;

std::vector<Twine_गाठ> StaticTwines::nodes_;

शून्य StaticTwines::init() {
	जर (ready())
		परत;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	साठी (स्थिरांक अक्षर *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

स्थिरांक Twine_गाठ &StaticTwines::गाठ(माप_प्रकार idx) { परत nodes_[idx]; }
तार्किक StaticTwines::ready() { परत nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
पूर्णांक twine_gc_मोजणी;

std::string IdString::handle_token() स्थिरांक {
	परत stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

माप_प्रकार IdString::handle_token_prefix(std::string_view token, तार्किक &is_public) {
	जर (token.substr(0, 5) == "$pub@") {
		is_public = सत्य;
		परत 5;
	}
	जर (token.substr(0, 6) == "$priv@") {
		is_public = असत्य;
		परत 6;
	}
	परत 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	परत result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	परत static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

शून्य SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	जर (len_ > INLINE_CAP) {
		ptr_ = नवीन अक्षर[len_];
		स_नकल(ptr_, content.data(), len_);
	} अन्यथा {
		स_नकल(inl_, content.data(), len_);
	}
}

शून्य SmallString::release()
{
	जर (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(स्थिरांक SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	स_नकल(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::चालक=(स्थिरांक SmallString &other)
{
	जर (हे != &other)
		store(other.view());
	परत *हे;
}

SmallString &SmallString::चालक=(SmallString &&other) noexcept
{
	जर (हे == &other)
		परत *हे;
	release();
	स_नकल(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	परत *हे;
}

SmallString::~SmallString() { release(); }

Twine_गाठ::Twine_गाठ(Twine_गाठ &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_गाठ &Twine_गाठ::चालक=(Twine_गाठ &&other) noexcept
{
	जर (हे == &other)
		परत *हे;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	परत *हे;
}

std::string TwineSpec::content_str() स्थिरांक {
	जर (स्वतः *leaf = std::get_if<Leaf>(&data))
		परत leaf->s;
	स्वतः &autosfx = std::get<AutoSuffix>(data);
	परत *autosfx.prefix + autosfx.tail;
}

स्थिर std::pair<std::string_view, तार्किक> unescape_name(std::string_view name) {
	जर (!name.empty() && name[0] == '\\')
		परत {name.substr(1), सत्य};
	परत {name, असत्य};
}

स्थिर शून्य check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	साठी (अक्षर ch : name)
		जर ((अचिन्हित)ch <= (अचिन्हित)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(स्थिरांक TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::चालक=(स्थिरांक TwinePool& other) {
	जर (हे == &other)
		परत *हे;
	HashConsPool::चालक=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	परत *हे;
}

TwinePool& TwinePool::चालक=(TwinePool&& other) {
	जर (हे == &other)
		परत *हे;
	HashConsPool::चालक=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	परत *हे;
}

तार्किक TwinePool::owns(IdString ref) स्थिरांक {
	परत ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) स्थिरांक {
	जर (ref == IdString::Null || ID::is_static(ref))
		परत ref;
	परत ref.stamped(serial_);
}

शून्य TwinePool::check_owned(IdString ref) स्थिरांक {
	जर constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

स्थिरांक Twine_गाठ& TwinePool::चालक[](IdString ref) स्थिरांक {
	check_owned(ref);
	परत HashConsPool::चालक[](ref);
}

माप_प्रकार TwinePool::gc(pool<IdString>& roots) {
	साठी (स्वतः &it : auto_prefixes)
		roots.घाला(it.second);
	परत HashConsPool::gc(roots);
}

स्थिरांक Twine_गाठ& TwinePool::static_गाठ(माप_प्रकार idx) { परत StaticTwines::गाठ(idx); }
शून्य TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

माप_प्रकार TwinePool::next_serial() {
	स्थिर माप_प्रकार counter = 0;
	माप_प्रकार serial = ++counter;
	जर constexpr (IdString::MAX_SERIAL == 0)
		परत serial;
	अन्यथा
		परत serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_गाठ t) {
	जर (free_list.empty() && STATIC_मोजणी + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_मोजणी);
	परत HashConsPool::add_inner(std::move(t));
}

शून्य TwinePool::dump(IdString ref, std::ostream& os) स्थिरांक {
	स्थिरांक Twine_गाठ& twine = (*हे)[ref];
	निवड (twine.kind()) {
	स्थिती Twine_गाठ::Kind::Dead:
		os << "Dead()";
		थांबा;
	स्थिती Twine_गाठ::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		थांबा;
	स्थिती Twine_गाठ::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		थांबा;
	}
	जर (ref.isPublic())
		os << " pub";
}

शून्य TwinePool::append_str(IdString ref, std::string& out) स्थिरांक {
	जर (ref == IdString::Null)
		परत;
	जर (ref.isPublic())
		out += '\\';
	स्थिरांक Twine_गाठ& twine = (*हे)[ref];
	निवड (twine.kind()) {
	स्थिती Twine_गाठ::Kind::Dead:
		थांबा;
	स्थिती Twine_गाठ::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	स्थिती Twine_गाठ::Kind::Leaf:
		out += twine.text();
		थांबा;
	}
}

std::string TwinePool::str(IdString ref) स्थिरांक {
	std::string out;
	append_str(ref, out);
	परत out;
}

std::string TwinePool::unescaped_str(IdString ref) स्थिरांक {
	परत str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) स्थिरांक {
	uint32_t hash = prefix == Twine_गाठ::NO_PREFIX ? 0 : (*हे)[IdString(prefix)].hash();
	परत Twine_गाठ::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) स्थिरांक {
	परत find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	जर (ref != IdString::Null)
		परत ref;
	परत add_inner(Twine_गाठ{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) स्थिरांक {
	जर (name.empty())
		परत IdString::Null;
	check_name(name);
	स्वतः [content, is_public] = unescape_name(name);
	परत stamp(find_content(Twine_गाठ::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) स्थिरांक {
	जर (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		परत stamp(find_content(Twine_गाठ::NO_PREFIX, t.content_str()));
	जर (स्वतः *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		परत stamp(find_content(Twine_गाठ::NO_PREFIX, leaf->s));
	स्थिरांक TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	परत stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	जर (स्वतः *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		परत add(auto_prefix(ap->prefix), ap->tail);
	जर (स्वतः *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		परत stamp(intern(Twine_गाठ::NO_PREFIX, leaf->s));
	स्थिरांक TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	परत add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(स्थिरांक std::string *prefix) {
	स्वतः it = auto_prefixes.find(prefix);
	जर (it != auto_prefixes.end())
		परत it->second;
	IdString ref = intern(Twine_गाठ::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	परत ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	परत stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	जर (s.empty())
		परत IdString::Null;
	check_name(s);
	स्वतः [content, is_public] = unescape_name(s);
	परत stamp(intern(Twine_गाठ::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(स्थिरांक TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	जर (found != IdString::Null || ref == IdString::Null)
		परत found;
	स्थिरांक Twine_गाठ& t = src[ref.untag()];
	जर (t.is_dead())
		परत IdString::Null;
	uint32_t prefix = Twine_गाठ::NO_PREFIX;
	जर (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	परत stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) स्थिरांक {
	परत "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) स्थिरांक {
	जर (token.size() < 2 || token[0] != '#')
		परत IdString::Null;
	माप_प्रकार sep = token.find(':');
	जर (sep == std::string_view::npos)
		परत IdString::Null;
	माप_प्रकार fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	साठी (पूर्णांक i = 0; i < 2; i++) {
		जर (parts[i].empty())
			परत IdString::Null;
		साठी (अक्षर c : parts[i]) {
			जर (c < '0' || c > '9')
				परत IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	जर (fields[0] != serial_)
		परत IdString::Null;
	IdString ref(fields[1]);
	जर (ref == IdString::Null)
		परत IdString::Null;
	परत is_live(ref) ? ref : IdString::Null;
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
	TwineSegments(स्थिरांक TwinePool &pool, IdString ref);
	TwineSegments(स्थिरांक TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	शून्य advance(माप_प्रकार n);
	माप_प्रकार total_size() स्थिरांक;
खाजगी:
	शून्य ढकला(std::string_view seg);
	std::string_view *segs();
	स्थिरांक std::string_view *segs() स्थिरांक;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	माप_प्रकार मोजणी = 0;
	माप_प्रकार pos = 0;
	माप_प्रकार off = 0;
};

TwineSegments::TwineSegments(स्थिरांक TwinePool &pool, IdString ref)
{
	जर (ref == IdString::Null)
		परत;
	साठी (IdString cur = ref.untag(); ;) {
		स्थिरांक Twine_गाठ &t = pool[cur];
		निवड (t.kind()) {
		स्थिती Twine_गाठ::Kind::Suffix:
			ढकला(t.text());
			cur = t.prefix();
			सुरूठेवा;
		स्थिती Twine_गाठ::Kind::Leaf:
			ढकला(t.text());
			थांबा;
		स्थिती Twine_गाठ::Kind::Dead:
			थांबा;
		}
		थांबा;
	}
	जर (ref.isPublic())
		ढकला("\\");
	std::reverse(segs(), segs() + मोजणी);
}

TwineSegments::TwineSegments(स्थिरांक TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	ढकला(tail);
}

std::string_view TwineSegments::peek()
{
	पर्यंत (pos < मोजणी) {
		std::string_view seg = segs()[pos];
		जर (off < seg.size())
			परत seg.substr(off);
		pos++;
		off = 0;
	}
	परत {};
}

शून्य TwineSegments::advance(माप_प्रकार n) { off += n; }

माप_प्रकार TwineSegments::total_size() स्थिरांक
{
	माप_प्रकार total = 0;
	साठी (माप_प्रकार i = 0; i < मोजणी; i++)
		total += segs()[i].size();
	परत total;
}

शून्य TwineSegments::ढकला(std::string_view seg)
{
	जर (spill.empty() && मोजणी < SEGMENT_INLINE_DEPTH) {
		inline_segs[मोजणी++] = seg;
		परत;
	}
	जर (spill.empty())
		spill.assign(inline_segs, inline_segs + मोजणी);
	spill.ढकला_back(seg);
	मोजणी++;
}

std::string_view *TwineSegments::segs() { परत spill.empty() ? inline_segs : spill.data(); }
स्थिरांक std::string_view *TwineSegments::segs() स्थिरांक { परत spill.empty() ? inline_segs : spill.data(); }

माप_प्रकार TwinePool::str_size(IdString ref) स्थिरांक
{
	परत TwineSegments(*हे, ref).total_size();
}

तार्किक TwinePool::begins_with(IdString ref, std::string_view prefix) स्थिरांक
{
	TwineSegments segs(*हे, ref);
	पर्यंत (!prefix.empty()) {
		std::string_view seg = segs.peek();
		जर (seg.empty())
			परत असत्य;
		माप_प्रकार n = std::min(seg.size(), prefix.size());
		जर (std::स_भेद(seg.data(), prefix.data(), n) != 0)
			परत असत्य;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	परत सत्य;
}

तार्किक TwinePool::name_equal(IdString ref, std::string_view name) स्थिरांक
{
	परत str_size(ref) == name.size() && begins_with(ref, name);
}

स्थिर पूर्णांक compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	पर्यंत (सत्य) {
		std::string_view x = sa.peek(), y = sb.peek();
		जर (x.empty() || y.empty())
			परत x.empty() ? (y.empty() ? 0 : -1) : 1;
		माप_प्रकार n = std::min(x.size(), y.size());
		जर (पूर्णांक diff = std::स_भेद(x.data(), y.data(), n); diff != 0)
			परत diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) स्थिरांक {
	परत find_hashed(hash, [&](IdString ref) {
		स्थिरांक Twine_गाठ &t = (*हे)[ref];
		जर (t == Twine_गाठ::किल्ली_2{prefix, text})
			परत सत्य;
		IdString head = prefix == Twine_गाठ::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*हे, ref), sb(*हे, head, text);
		परत compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(स्थिरांक TwinePool& src, IdString ref) स्थिरांक {
	जर (ref == IdString::Null || ID::is_static(ref))
		परत ref;
	IdString idx = ref.untag();
	स्थिरांक Twine_गाठ& t = src[idx];
	जर (t.is_dead())
		परत IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*हे, cand), sb(src, idx);
		परत compare_segments(sa, sb) == 0;
	});
	परत stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) स्थिरांक {
	स्थिरांक Twine_गाठ &t = (*हे)[ref];
	जर (!t.is_suffix())
		परत IdString::Null;
	परत stamp(t.prefix().tag(ref.isPublic()));
}

पूर्णांक TwinePool::compare_by_name(IdString a, IdString b) स्थिरांक
{
	जर (a == b)
		परत 0;
	जर (a == IdString::Null)
		परत -1;
	जर (b == IdString::Null)
		परत 1;

	जर (a.isPublic() == b.isPublic()) {
		स्थिरांक Twine_गाठ &ta = (*हे)[a.untag()];
		स्थिरांक Twine_गाठ &tb = (*हे)[b.untag()];
		जर (ta.is_leaf() && tb.is_leaf())
			परत ta.text().compare(tb.text());
		जर (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			परत ta.text().compare(tb.text());
	}

	TwineSegments sa(*हे, a), sb(*हे, b);
	परत compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
