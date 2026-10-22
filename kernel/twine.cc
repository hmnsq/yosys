#অন্তর্ভাৱ "kernel/twine.h"
#অন্তর্ভাৱ "kernel/hashcons_impl.h"
#অন্তর্ভাৱ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template সংরচনা HashConsPool<TwinePool, Twine_গ্ৰন্থি, IdString>;

std::vector<Twine_গ্ৰন্থি> StaticTwines::nodes_;

শূন্য StaticTwines::init() {
	যদি (ready())
		ঘূৰাই;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	বাবে (ধ্রুৱ ৱর্ণ *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ধ্রুৱ Twine_গ্ৰন্থি &StaticTwines::গ্ৰন্থি(size_t idx) { ঘূৰাই nodes_[idx]; }
তার্কিক StaticTwines::ready() { ঘূৰাই nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
পূর্ণাঙ্ক twine_gc_গুনতি;

std::string IdString::handle_token() ধ্রুৱ {
	ঘূৰাই stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, তার্কিক &is_public) {
	যদি (token.substr(0, 5) == "$pub@") {
		is_public = সঁচা;
		ঘূৰাই 5;
	}
	যদি (token.substr(0, 6) == "$priv@") {
		is_public = মিছা;
		ঘূৰাই 6;
	}
	ঘূৰাই 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	ঘূৰাই result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	ঘূৰাই static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

শূন্য SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	যদি (len_ > INLINE_CAP) {
		ptr_ = নূতন ৱর্ণ[len_];
		memcpy(ptr_, content.data(), len_);
	} অন্যথা {
		memcpy(inl_, content.data(), len_);
	}
}

শূন্য SmallString::release()
{
	যদি (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ধ্রুৱ SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ধ্রুৱ SmallString &other)
{
	যদি (এতত্ != &other)
		store(other.view());
	ঘূৰাই *এতত্;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	যদি (এতত্ == &other)
		ঘূৰাই *এতত্;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	ঘূৰাই *এতত্;
}

SmallString::~SmallString() { release(); }

Twine_গ্ৰন্থি::Twine_গ্ৰন্থি(Twine_গ্ৰন্থি &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_গ্ৰন্থি &Twine_গ্ৰন্থি::operator=(Twine_গ্ৰন্থি &&other) noexcept
{
	যদি (এতত্ == &other)
		ঘূৰাই *এতত্;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	ঘূৰাই *এতত্;
}

std::string TwineSpec::content_str() ধ্রুৱ {
	যদি (auto *leaf = std::get_if<Leaf>(&data))
		ঘূৰাই leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	ঘূৰাই *autosfx.prefix + autosfx.tail;
}

স্থির std::pair<std::string_view, তার্কিক> unescape_name(std::string_view name) {
	যদি (!name.empty() && name[0] == '\\')
		ঘূৰাই {name.substr(1), সঁচা};
	ঘূৰাই {name, মিছা};
}

স্থির শূন্য check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	বাবে (ৱর্ণ ch : name)
		যদি ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ধ্রুৱ TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ধ্রুৱ TwinePool& other) {
	যদি (এতত্ == &other)
		ঘূৰাই *এতত্;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	ঘূৰাই *এতত্;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	যদি (এতত্ == &other)
		ঘূৰাই *এতত্;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	ঘূৰাই *এতত্;
}

তার্কিক TwinePool::owns(IdString ref) ধ্রুৱ {
	ঘূৰাই ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ধ্রুৱ {
	যদি (ref == IdString::Null || ID::is_static(ref))
		ঘূৰাই ref;
	ঘূৰাই ref.stamped(serial_);
}

শূন্য TwinePool::check_owned(IdString ref) ধ্রুৱ {
	যদি constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ধ্রুৱ Twine_গ্ৰন্থি& TwinePool::operator[](IdString ref) ধ্রুৱ {
	check_owned(ref);
	ঘূৰাই HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	বাবে (auto &it : auto_prefixes)
		roots.ঢোকাও(it.second);
	ঘূৰাই HashConsPool::gc(roots);
}

ধ্রুৱ Twine_গ্ৰন্থি& TwinePool::static_গ্ৰন্থি(size_t idx) { ঘূৰাই StaticTwines::গ্ৰন্থি(idx); }
শূন্য TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	স্থির size_t counter = 0;
	size_t serial = ++counter;
	যদি constexpr (IdString::MAX_SERIAL == 0)
		ঘূৰাই serial;
	অন্যথা
		ঘূৰাই serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_গ্ৰন্থি t) {
	যদি (free_list.empty() && STATIC_গুনতি + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_গুনতি);
	ঘূৰাই HashConsPool::add_inner(std::move(t));
}

শূন্য TwinePool::dump(IdString ref, std::ostream& os) ধ্রুৱ {
	ধ্রুৱ Twine_গ্ৰন্থি& twine = (*এতত্)[ref];
	ৱিকল্প (twine.kind()) {
	পক্ষ Twine_গ্ৰন্থি::Kind::Dead:
		os << "Dead()";
		ভঙ্গ;
	পক্ষ Twine_গ্ৰন্থি::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		ভঙ্গ;
	পক্ষ Twine_গ্ৰন্থি::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		ভঙ্গ;
	}
	যদি (ref.isPublic())
		os << " pub";
}

শূন্য TwinePool::append_str(IdString ref, std::string& out) ধ্রুৱ {
	যদি (ref == IdString::Null)
		ঘূৰাই;
	যদি (ref.isPublic())
		out += '\\';
	ধ্রুৱ Twine_গ্ৰন্থি& twine = (*এতত্)[ref];
	ৱিকল্প (twine.kind()) {
	পক্ষ Twine_গ্ৰন্থি::Kind::Dead:
		ভঙ্গ;
	পক্ষ Twine_গ্ৰন্থি::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	পক্ষ Twine_গ্ৰন্থি::Kind::Leaf:
		out += twine.text();
		ভঙ্গ;
	}
}

std::string TwinePool::str(IdString ref) ধ্রুৱ {
	std::string out;
	append_str(ref, out);
	ঘূৰাই out;
}

std::string TwinePool::unescaped_str(IdString ref) ধ্রুৱ {
	ঘূৰাই str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ধ্রুৱ {
	uint32_t hash = prefix == Twine_গ্ৰন্থি::NO_PREFIX ? 0 : (*এতত্)[IdString(prefix)].hash();
	ঘূৰাই Twine_গ্ৰন্থি::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ধ্রুৱ {
	ঘূৰাই find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	যদি (ref != IdString::Null)
		ঘূৰাই ref;
	ঘূৰাই add_inner(Twine_গ্ৰন্থি{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ধ্রুৱ {
	যদি (name.empty())
		ঘূৰাই IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	ঘূৰাই stamp(find_content(Twine_গ্ৰন্থি::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ধ্রুৱ {
	যদি (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		ঘূৰাই stamp(find_content(Twine_গ্ৰন্থি::NO_PREFIX, t.content_str()));
	যদি (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ঘূৰাই stamp(find_content(Twine_গ্ৰন্থি::NO_PREFIX, leaf->s));
	ধ্রুৱ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ঘূৰাই stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	যদি (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		ঘূৰাই add(auto_prefix(ap->prefix), ap->tail);
	যদি (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		ঘূৰাই stamp(intern(Twine_গ্ৰন্থি::NO_PREFIX, leaf->s));
	ধ্রুৱ TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	ঘূৰাই add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ধ্রুৱ std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	যদি (it != auto_prefixes.end())
		ঘূৰাই it->second;
	IdString ref = intern(Twine_গ্ৰন্থি::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	ঘূৰাই ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	ঘূৰাই stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	যদি (s.empty())
		ঘূৰাই IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	ঘূৰাই stamp(intern(Twine_গ্ৰন্থি::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ধ্রুৱ TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	যদি (found != IdString::Null || ref == IdString::Null)
		ঘূৰাই found;
	ধ্রুৱ Twine_গ্ৰন্থি& t = src[ref.untag()];
	যদি (t.is_dead())
		ঘূৰাই IdString::Null;
	uint32_t prefix = Twine_গ্ৰন্থি::NO_PREFIX;
	যদি (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	ঘূৰাই stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ধ্রুৱ {
	ঘূৰাই "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ধ্রুৱ {
	যদি (token.size() < 2 || token[0] != '#')
		ঘূৰাই IdString::Null;
	size_t sep = token.find(':');
	যদি (sep == std::string_view::npos)
		ঘূৰাই IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	বাবে (পূর্ণাঙ্ক i = 0; i < 2; i++) {
		যদি (parts[i].empty())
			ঘূৰাই IdString::Null;
		বাবে (ৱর্ণ c : parts[i]) {
			যদি (c < '0' || c > '9')
				ঘূৰাই IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	যদি (fields[0] != serial_)
		ঘূৰাই IdString::Null;
	IdString ref(fields[1]);
	যদি (ref == IdString::Null)
		ঘূৰাই IdString::Null;
	ঘূৰাই is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
সংরচনা TwineSegments {
	স্থির constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ধ্রুৱ TwinePool &pool, IdString ref);
	TwineSegments(ধ্রুৱ TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	শূন্য advance(size_t n);
	size_t total_size() ধ্রুৱ;
গুপ্ত:
	শূন্য ঠেলো(std::string_view seg);
	std::string_view *segs();
	ধ্রুৱ std::string_view *segs() ধ্রুৱ;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t গুনতি = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ধ্রুৱ TwinePool &pool, IdString ref)
{
	যদি (ref == IdString::Null)
		ঘূৰাই;
	বাবে (IdString cur = ref.untag(); ;) {
		ধ্রুৱ Twine_গ্ৰন্থি &t = pool[cur];
		ৱিকল্প (t.kind()) {
		পক্ষ Twine_গ্ৰন্থি::Kind::Suffix:
			ঠেলো(t.text());
			cur = t.prefix();
			অনুৱর্ত;
		পক্ষ Twine_গ্ৰন্থি::Kind::Leaf:
			ঠেলো(t.text());
			ভঙ্গ;
		পক্ষ Twine_গ্ৰন্থি::Kind::Dead:
			ভঙ্গ;
		}
		ভঙ্গ;
	}
	যদি (ref.isPublic())
		ঠেলো("\\");
	std::reverse(segs(), segs() + গুনতি);
}

TwineSegments::TwineSegments(ধ্রুৱ TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	ঠেলো(tail);
}

std::string_view TwineSegments::peek()
{
	যেতিয়ালৈ (pos < গুনতি) {
		std::string_view seg = segs()[pos];
		যদি (off < seg.size())
			ঘূৰাই seg.substr(off);
		pos++;
		off = 0;
	}
	ঘূৰাই {};
}

শূন্য TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ধ্রুৱ
{
	size_t total = 0;
	বাবে (size_t i = 0; i < গুনতি; i++)
		total += segs()[i].size();
	ঘূৰাই total;
}

শূন্য TwineSegments::ঠেলো(std::string_view seg)
{
	যদি (spill.empty() && গুনতি < SEGMENT_INLINE_DEPTH) {
		inline_segs[গুনতি++] = seg;
		ঘূৰাই;
	}
	যদি (spill.empty())
		spill.assign(inline_segs, inline_segs + গুনতি);
	spill.ঠেলো_back(seg);
	গুনতি++;
}

std::string_view *TwineSegments::segs() { ঘূৰাই spill.empty() ? inline_segs : spill.data(); }
ধ্রুৱ std::string_view *TwineSegments::segs() ধ্রুৱ { ঘূৰাই spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ধ্রুৱ
{
	ঘূৰাই TwineSegments(*এতত্, ref).total_size();
}

তার্কিক TwinePool::begins_with(IdString ref, std::string_view prefix) ধ্রুৱ
{
	TwineSegments segs(*এতত্, ref);
	যেতিয়ালৈ (!prefix.empty()) {
		std::string_view seg = segs.peek();
		যদি (seg.empty())
			ঘূৰাই মিছা;
		size_t n = std::min(seg.size(), prefix.size());
		যদি (std::memcmp(seg.data(), prefix.data(), n) != 0)
			ঘূৰাই মিছা;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	ঘূৰাই সঁচা;
}

তার্কিক TwinePool::name_equal(IdString ref, std::string_view name) ধ্রুৱ
{
	ঘূৰাই str_size(ref) == name.size() && begins_with(ref, name);
}

স্থির পূর্ণাঙ্ক compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	যেতিয়ালৈ (সঁচা) {
		std::string_view x = sa.peek(), y = sb.peek();
		যদি (x.empty() || y.empty())
			ঘূৰাই x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		যদি (পূর্ণাঙ্ক diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			ঘূৰাই diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ধ্রুৱ {
	ঘূৰাই find_hashed(hash, [&](IdString ref) {
		ধ্রুৱ Twine_গ্ৰন্থি &t = (*এতত্)[ref];
		যদি (t == Twine_গ্ৰন্থি::চাবি_2{prefix, text})
			ঘূৰাই সঁচা;
		IdString head = prefix == Twine_গ্ৰন্থি::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*এতত্, ref), sb(*এতত্, head, text);
		ঘূৰাই compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ধ্রুৱ TwinePool& src, IdString ref) ধ্রুৱ {
	যদি (ref == IdString::Null || ID::is_static(ref))
		ঘূৰাই ref;
	IdString idx = ref.untag();
	ধ্রুৱ Twine_গ্ৰন্থি& t = src[idx];
	যদি (t.is_dead())
		ঘূৰাই IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*এতত্, cand), sb(src, idx);
		ঘূৰাই compare_segments(sa, sb) == 0;
	});
	ঘূৰাই stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ধ্রুৱ {
	ধ্রুৱ Twine_গ্ৰন্থি &t = (*এতত্)[ref];
	যদি (!t.is_suffix())
		ঘূৰাই IdString::Null;
	ঘূৰাই stamp(t.prefix().tag(ref.isPublic()));
}

পূর্ণাঙ্ক TwinePool::compare_by_name(IdString a, IdString b) ধ্রুৱ
{
	যদি (a == b)
		ঘূৰাই 0;
	যদি (a == IdString::Null)
		ঘূৰাই -1;
	যদি (b == IdString::Null)
		ঘূৰাই 1;

	যদি (a.isPublic() == b.isPublic()) {
		ধ্রুৱ Twine_গ্ৰন্থি &ta = (*এতত্)[a.untag()];
		ধ্রুৱ Twine_গ্ৰন্থি &tb = (*এতত্)[b.untag()];
		যদি (ta.is_leaf() && tb.is_leaf())
			ঘূৰাই ta.text().compare(tb.text());
		যদি (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			ঘূৰাই ta.text().compare(tb.text());
	}

	TwineSegments sa(*এতত্, a), sb(*এতত্, b);
	ঘূৰাই compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
