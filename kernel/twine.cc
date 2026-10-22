#অন্তর্ভাব "kernel/twine.h"
#অন্তর্ভাব "kernel/hashcons_impl.h"
#অন্তর্ভাব "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template কাঠামো HashConsPool<TwinePool, Twine_গ্রন্থি, IdString>;

std::vector<Twine_গ্রন্থি> StaticTwines::nodes_;

শূন্য StaticTwines::init() {
	যদি (ready())
		ফেরত;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	জন্য (ধ্রুবক অক্ষর *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ধ্রুবক Twine_গ্রন্থি &StaticTwines::গ্রন্থি(আকার_t idx) { ফেরত nodes_[idx]; }
যৌক্তিক StaticTwines::ready() { ফেরত nodes_.আকার_2() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
পূর্ণসংখ্যা twine_gc_গুনতি;

std::string IdString::handle_token() ধ্রুবক {
	ফেরত stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

আকার_t IdString::handle_token_prefix(std::string_view token, যৌক্তিক &is_public) {
	যদি (token.substr(0, 5) == "$pub@") {
		is_public = সত্য;
		ফেরত 5;
	}
	যদি (token.substr(0, 6) == "$priv@") {
		is_public = মিথ্যা;
		ফেরত 6;
	}
	ফেরত 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string ফলাফল = ref.isPublic() ? "\\" : "";
	ফলাফল += static_names[idx.raw()];
	ফেরত ফলাফল;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ফেরত static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : তথ্য(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : তথ্য(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : তথ্য(std::move(v)) {}

শূন্য SmallString::store(std::string_view content)
{
	log_assert(content.আকার_2() <= MAX_LEN);
	release();
	len_ = content.আকার_2();
	যদি (len_ > INLINE_CAP) {
		ptr_ = নতুন অক্ষর[len_];
		memcpy(ptr_, content.তথ্য(), len_);
	} অন্যথা {
		memcpy(inl_, content.তথ্য(), len_);
	}
}

শূন্য SmallString::release()
{
	যদি (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ধ্রুবক SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ধ্রুবক SmallString &other)
{
	যদি (এই != &other)
		store(other.view());
	ফেরত *এই;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	যদি (এই == &other)
		ফেরত *এই;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ফেরত *এই;
}

SmallString::~SmallString() { release(); }

Twine_গ্রন্থি::Twine_গ্রন্থি(Twine_গ্রন্থি &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_গ্রন্থি &Twine_গ্রন্থি::operator=(Twine_গ্রন্থি &&other) noexcept
{
	যদি (এই == &other)
		ফেরত *এই;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ফেরত *এই;
}

std::string TwineSpec::content_str() ধ্রুবক {
	যদি (auto *leaf = std::get_if<Leaf>(&তথ্য))
		ফেরত leaf->s;
	auto &autosfx = std::get<AutoSuffix>(তথ্য);
	ফেরত *autosfx.prefix + autosfx.লেজ;
}

স্থির std::pair<std::string_view, যৌক্তিক> unescape_name(std::string_view name) {
	যদি (!name.empty() && name[0] == '\\')
		ফেরত {name.substr(1), সত্য};
	ফেরত {name, মিথ্যা};
}

স্থির শূন্য check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	জন্য (অক্ষর ch : name)
		যদি ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(পরবর্তী_serial()) {}
TwinePool::TwinePool(ধ্রুবক TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(পরবর্তী_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(পরবর্তী_serial()) {}

TwinePool& TwinePool::operator=(ধ্রুবক TwinePool& other) {
	যদি (এই == &other)
		ফেরত *এই;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = পরবর্তী_serial();
	ফেরত *এই;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	যদি (এই == &other)
		ফেরত *এই;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = পরবর্তী_serial();
	ফেরত *এই;
}

যৌক্তিক TwinePool::owns(IdString ref) ধ্রুবক {
	ফেরত ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ধ্রুবক {
	যদি (ref == IdString::Null || ID::is_static(ref))
		ফেরত ref;
	ফেরত ref.stamped(serial_);
}

শূন্য TwinePool::check_owned(IdString ref) ধ্রুবক {
	যদি constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ধ্রুবক Twine_গ্রন্থি& TwinePool::operator[](IdString ref) ধ্রুবক {
	check_owned(ref);
	ফেরত HashConsPool::operator[](ref);
}

আকার_t TwinePool::gc(pool<IdString>& roots) {
	জন্য (auto &it : auto_prefixes)
		roots.ঢোকাও(it.second);
	ফেরত HashConsPool::gc(roots);
}

ধ্রুবক Twine_গ্রন্থি& TwinePool::static_গ্রন্থি(আকার_t idx) { ফেরত StaticTwines::গ্রন্থি(idx); }
শূন্য TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

আকার_t TwinePool::পরবর্তী_serial() {
	স্থির আকার_t গণক = 0;
	আকার_t serial = ++গণক;
	যদি constexpr (IdString::MAX_SERIAL == 0)
		ফেরত serial;
	অন্যথা
		ফেরত serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_গ্রন্থি t) {
	যদি (free_list.empty() && STATIC_গুনতি + backing.আকার_2() > IdString::MAX_সূচক)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_সূচক - STATIC_গুনতি);
	ফেরত HashConsPool::add_inner(std::move(t));
}

শূন্য TwinePool::dump(IdString ref, std::ostream& os) ধ্রুবক {
	ধ্রুবক Twine_গ্রন্থি& twine = (*এই)[ref];
	নির্বাচন (twine.kind()) {
	ক্ষেত্র Twine_গ্রন্থি::Kind::Dead:
		os << "Dead()";
		বিরতি;
	ক্ষেত্র Twine_গ্রন্থি::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		বিরতি;
	ক্ষেত্র Twine_গ্রন্থি::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		বিরতি;
	}
	যদি (ref.isPublic())
		os << " pub";
}

শূন্য TwinePool::append_str(IdString ref, std::string& out) ধ্রুবক {
	যদি (ref == IdString::Null)
		ফেরত;
	যদি (ref.isPublic())
		out += '\\';
	ধ্রুবক Twine_গ্রন্থি& twine = (*এই)[ref];
	নির্বাচন (twine.kind()) {
	ক্ষেত্র Twine_গ্রন্থি::Kind::Dead:
		বিরতি;
	ক্ষেত্র Twine_গ্রন্থি::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	ক্ষেত্র Twine_গ্রন্থি::Kind::Leaf:
		out += twine.text();
		বিরতি;
	}
}

std::string TwinePool::str(IdString ref) ধ্রুবক {
	std::string out;
	append_str(ref, out);
	ফেরত out;
}

std::string TwinePool::unescaped_str(IdString ref) ধ্রুবক {
	ফেরত str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ধ্রুবক {
	uint32_t hash = prefix == Twine_গ্রন্থি::NO_PREFIX ? 0 : (*এই)[IdString(prefix)].hash();
	ফেরত Twine_গ্রন্থি::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ধ্রুবক {
	ফেরত find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	যদি (ref != IdString::Null)
		ফেরত ref;
	ফেরত add_inner(Twine_গ্রন্থি{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ধ্রুবক {
	যদি (name.empty())
		ফেরত IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ফেরত stamp(find_content(Twine_গ্রন্থি::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ধ্রুবক {
	যদি (std::holds_alternative<TwineSpec::AutoSuffix>(t.তথ্য))
		ফেরত stamp(find_content(Twine_গ্রন্থি::NO_PREFIX, t.content_str()));
	যদি (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.তথ্য))
		ফেরত stamp(find_content(Twine_গ্রন্থি::NO_PREFIX, leaf->s));
	ধ্রুবক TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.তথ্য);
	ফেরত stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.লেজ).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	যদি (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.তথ্য))
		ফেরত add(auto_prefix(ap->prefix), ap->লেজ);
	যদি (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.তথ্য))
		ফেরত stamp(intern(Twine_গ্রন্থি::NO_PREFIX, leaf->s));
	ধ্রুবক TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.তথ্য);
	ফেরত add(sfx.prefix, sfx.লেজ);
}

IdString TwinePool::auto_prefix(ধ্রুবক std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	যদি (it != auto_prefixes.end())
		ফেরত it->second;
	IdString ref = intern(Twine_গ্রন্থি::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ফেরত ref;
}

IdString TwinePool::add(IdString prefix, std::string_view লেজ) {
	ফেরত stamp(intern((uint32_t)prefix.untag().raw(), লেজ).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	যদি (s.empty())
		ফেরত IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ফেরত stamp(intern(Twine_গ্রন্থি::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ধ্রুবক TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	যদি (found != IdString::Null || ref == IdString::Null)
		ফেরত found;
	ধ্রুবক Twine_গ্রন্থি& t = src[ref.untag()];
	যদি (t.is_dead())
		ফেরত IdString::Null;
	uint32_t prefix = Twine_গ্রন্থি::NO_PREFIX;
	যদি (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ফেরত stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ধ্রুবক {
	ফেরত "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ধ্রুবক {
	যদি (token.আকার_2() < 2 || token[0] != '#')
		ফেরত IdString::Null;
	আকার_t sep = token.find(':');
	যদি (sep == std::string_view::npos)
		ফেরত IdString::Null;
	আকার_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	জন্য (পূর্ণসংখ্যা i = 0; i < 2; i++) {
		যদি (parts[i].empty())
			ফেরত IdString::Null;
		জন্য (অক্ষর c : parts[i]) {
			যদি (c < '0' || c > '9')
				ফেরত IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	যদি (fields[0] != serial_)
		ফেরত IdString::Null;
	IdString ref(fields[1]);
	যদি (ref == IdString::Null)
		ফেরত IdString::Null;
	ফেরত is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
কাঠামো TwineSegments {
	স্থির constexpr আকার_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ধ্রুবক TwinePool &pool, IdString ref);
	TwineSegments(ধ্রুবক TwinePool &pool, IdString শীর্ষ_2, std::string_view লেজ);
	std::string_view peek();
	শূন্য advance(আকার_t n);
	আকার_t যোগ_আকার() ধ্রুবক;
ব্যক্তিগত:
	শূন্য ঠেলো(std::string_view seg);
	std::string_view *segs();
	ধ্রুবক std::string_view *segs() ধ্রুবক;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	আকার_t গুনতি = 0;
	আকার_t pos = 0;
	আকার_t off = 0;
};

TwineSegments::TwineSegments(ধ্রুবক TwinePool &pool, IdString ref)
{
	যদি (ref == IdString::Null)
		ফেরত;
	জন্য (IdString cur = ref.untag(); ;) {
		ধ্রুবক Twine_গ্রন্থি &t = pool[cur];
		নির্বাচন (t.kind()) {
		ক্ষেত্র Twine_গ্রন্থি::Kind::Suffix:
			ঠেলো(t.text());
			cur = t.prefix();
			চালু;
		ক্ষেত্র Twine_গ্রন্থি::Kind::Leaf:
			ঠেলো(t.text());
			বিরতি;
		ক্ষেত্র Twine_গ্রন্থি::Kind::Dead:
			বিরতি;
		}
		বিরতি;
	}
	যদি (ref.isPublic())
		ঠেলো("\\");
	std::reverse(segs(), segs() + গুনতি);
}

TwineSegments::TwineSegments(ধ্রুবক TwinePool &pool, IdString শীর্ষ_2, std::string_view লেজ)
	: TwineSegments(pool, শীর্ষ_2)
{
	ঠেলো(লেজ);
}

std::string_view TwineSegments::peek()
{
	যখন (pos < গুনতি) {
		std::string_view seg = segs()[pos];
		যদি (off < seg.আকার_2())
			ফেরত seg.substr(off);
		pos++;
		off = 0;
	}
	ফেরত {};
}

শূন্য TwineSegments::advance(আকার_t n) { off += n; }

আকার_t TwineSegments::যোগ_আকার() ধ্রুবক
{
	আকার_t যোগ_2 = 0;
	জন্য (আকার_t i = 0; i < গুনতি; i++)
		যোগ_2 += segs()[i].আকার_2();
	ফেরত যোগ_2;
}

শূন্য TwineSegments::ঠেলো(std::string_view seg)
{
	যদি (spill.empty() && গুনতি < SEGMENT_INLINE_DEPTH) {
		inline_segs[গুনতি++] = seg;
		ফেরত;
	}
	যদি (spill.empty())
		spill.assign(inline_segs, inline_segs + গুনতি);
	spill.ঠেলো_back(seg);
	গুনতি++;
}

std::string_view *TwineSegments::segs() { ফেরত spill.empty() ? inline_segs : spill.তথ্য(); }
ধ্রুবক std::string_view *TwineSegments::segs() ধ্রুবক { ফেরত spill.empty() ? inline_segs : spill.তথ্য(); }

আকার_t TwinePool::str_আকার(IdString ref) ধ্রুবক
{
	ফেরত TwineSegments(*এই, ref).যোগ_আকার();
}

যৌক্তিক TwinePool::begins_with(IdString ref, std::string_view prefix) ধ্রুবক
{
	TwineSegments segs(*এই, ref);
	যখন (!prefix.empty()) {
		std::string_view seg = segs.peek();
		যদি (seg.empty())
			ফেরত মিথ্যা;
		আকার_t n = std::min(seg.আকার_2(), prefix.আকার_2());
		যদি (std::memcmp(seg.তথ্য(), prefix.তথ্য(), n) != 0)
			ফেরত মিথ্যা;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ফেরত সত্য;
}

যৌক্তিক TwinePool::name_equal(IdString ref, std::string_view name) ধ্রুবক
{
	ফেরত str_আকার(ref) == name.আকার_2() && begins_with(ref, name);
}

স্থির পূর্ণসংখ্যা compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	যখন (সত্য) {
		std::string_view x = sa.peek(), y = sb.peek();
		যদি (x.empty() || y.empty())
			ফেরত x.empty() ? (y.empty() ? 0 : -1) : 1;
		আকার_t n = std::min(x.আকার_2(), y.আকার_2());
		যদি (পূর্ণসংখ্যা diff = std::memcmp(x.তথ্য(), y.তথ্য(), n); diff != 0)
			ফেরত diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ধ্রুবক {
	ফেরত find_hashed(hash, [&](IdString ref) {
		ধ্রুবক Twine_গ্রন্থি &t = (*এই)[ref];
		যদি (t == Twine_গ্রন্থি::চাবি_2{prefix, text})
			ফেরত সত্য;
		IdString শীর্ষ_2 = prefix == Twine_গ্রন্থি::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*এই, ref), sb(*এই, শীর্ষ_2, text);
		ফেরত compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ধ্রুবক TwinePool& src, IdString ref) ধ্রুবক {
	যদি (ref == IdString::Null || ID::is_static(ref))
		ফেরত ref;
	IdString idx = ref.untag();
	ধ্রুবক Twine_গ্রন্থি& t = src[idx];
	যদি (t.is_dead())
		ফেরত IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*এই, cand), sb(src, idx);
		ফেরত compare_segments(sa, sb) == 0;
	});
	ফেরত stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ধ্রুবক {
	ধ্রুবক Twine_গ্রন্থি &t = (*এই)[ref];
	যদি (!t.is_suffix())
		ফেরত IdString::Null;
	ফেরত stamp(t.prefix().tag(ref.isPublic()));
}

পূর্ণসংখ্যা TwinePool::compare_by_name(IdString a, IdString b) ধ্রুবক
{
	যদি (a == b)
		ফেরত 0;
	যদি (a == IdString::Null)
		ফেরত -1;
	যদি (b == IdString::Null)
		ফেরত 1;

	যদি (a.isPublic() == b.isPublic()) {
		ধ্রুবক Twine_গ্রন্থি &ta = (*এই)[a.untag()];
		ধ্রুবক Twine_গ্রন্থি &tb = (*এই)[b.untag()];
		যদি (ta.is_leaf() && tb.is_leaf())
			ফেরত ta.text().compare(tb.text());
		যদি (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ফেরত ta.text().compare(tb.text());
	}

	TwineSegments sa(*এই, a), sb(*এই, b);
	ফেরত compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
