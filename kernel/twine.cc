#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template بنية HashConsPool<TwinePool, Twine_عقدة, IdString>;

std::vector<Twine_عقدة> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	إذا (ready())
		إرجاع;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	لكل (ثابت حرف *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ثابت Twine_عقدة &StaticTwines::عقدة(size_t idx) { إرجاع nodes_[idx]; }
منطقي StaticTwines::ready() { إرجاع nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
صحيح twine_gc_تعداد;

std::string IdString::handle_token() ثابت {
	إرجاع stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, منطقي &is_public) {
	إذا (token.substr(0, 5) == "$pub@") {
		is_public = true;
		إرجاع 5;
	}
	إذا (token.substr(0, 6) == "$priv@") {
		is_public = خطأ;
		إرجاع 6;
	}
	إرجاع 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	إرجاع result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	إرجاع static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	إذا (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		memcpy(ptr_, content.data(), len_);
	} وإلا {
		memcpy(inl_, content.data(), len_);
	}
}

فراغ SmallString::release()
{
	إذا (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ثابت SmallString &other)
{
	إذا (هذا != &other)
		store(other.view());
	إرجاع *هذا;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	إذا (هذا == &other)
		إرجاع *هذا;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	إرجاع *هذا;
}

SmallString::~SmallString() { release(); }

Twine_عقدة::Twine_عقدة(Twine_عقدة &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_عقدة &Twine_عقدة::operator=(Twine_عقدة &&other) noexcept
{
	إذا (هذا == &other)
		إرجاع *هذا;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	إرجاع *هذا;
}

std::string TwineSpec::content_str() ثابت {
	إذا (auto *leaf = std::get_if<Leaf>(&data))
		إرجاع leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	إرجاع *autosfx.prefix + autosfx.tail;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	إذا (!name.empty() && name[0] == '\\')
		إرجاع {name.substr(1), true};
	إرجاع {name, خطأ};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	لكل (حرف ch : name)
		إذا ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ثابت TwinePool& other) {
	إذا (هذا == &other)
		إرجاع *هذا;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	إرجاع *هذا;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	إذا (هذا == &other)
		إرجاع *هذا;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	إرجاع *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	إرجاع ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		إرجاع ref;
	إرجاع ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	إذا constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_عقدة& TwinePool::operator[](IdString ref) ثابت {
	check_owned(ref);
	إرجاع HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	لكل (auto &it : auto_prefixes)
		roots.أدرج(it.second);
	إرجاع HashConsPool::gc(roots);
}

ثابت Twine_عقدة& TwinePool::static_عقدة(size_t idx) { إرجاع StaticTwines::عقدة(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	ساكن size_t counter = 0;
	size_t serial = ++counter;
	إذا constexpr (IdString::MAX_SERIAL == 0)
		إرجاع serial;
	وإلا
		إرجاع serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_عقدة t) {
	إذا (free_list.empty() && STATIC_تعداد + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_تعداد);
	إرجاع HashConsPool::add_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_عقدة& twine = (*هذا)[ref];
	بدل (twine.kind()) {
	حالة Twine_عقدة::Kind::Dead:
		os << "Dead()";
		اكسر;
	حالة Twine_عقدة::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		اكسر;
	حالة Twine_عقدة::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		اكسر;
	}
	إذا (ref.isPublic())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& out) ثابت {
	إذا (ref == IdString::Null)
		إرجاع;
	إذا (ref.isPublic())
		out += '\\';
	ثابت Twine_عقدة& twine = (*هذا)[ref];
	بدل (twine.kind()) {
	حالة Twine_عقدة::Kind::Dead:
		اكسر;
	حالة Twine_عقدة::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	حالة Twine_عقدة::Kind::Leaf:
		out += twine.text();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string out;
	append_str(ref, out);
	إرجاع out;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	إرجاع str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ثابت {
	uint32_t hash = prefix == Twine_عقدة::NO_PREFIX ? 0 : (*هذا)[IdString(prefix)].hash();
	إرجاع Twine_عقدة::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ثابت {
	إرجاع find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	إذا (ref != IdString::Null)
		إرجاع ref;
	إرجاع add_inner(Twine_عقدة{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ثابت {
	إذا (name.empty())
		إرجاع IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	إرجاع stamp(find_content(Twine_عقدة::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ثابت {
	إذا (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		إرجاع stamp(find_content(Twine_عقدة::NO_PREFIX, t.content_str()));
	إذا (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		إرجاع stamp(find_content(Twine_عقدة::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	إرجاع stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	إذا (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		إرجاع add(auto_prefix(ap->prefix), ap->tail);
	إذا (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		إرجاع stamp(intern(Twine_عقدة::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	إرجاع add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ثابت std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	إذا (it != auto_prefixes.end())
		إرجاع it->second;
	IdString ref = intern(Twine_عقدة::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	إرجاع ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	إرجاع stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	إذا (s.empty())
		إرجاع IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	إرجاع stamp(intern(Twine_عقدة::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	إذا (found != IdString::Null || ref == IdString::Null)
		إرجاع found;
	ثابت Twine_عقدة& t = src[ref.untag()];
	إذا (t.is_dead())
		إرجاع IdString::Null;
	uint32_t prefix = Twine_عقدة::NO_PREFIX;
	إذا (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	إرجاع stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	إرجاع "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	إذا (token.size() < 2 || token[0] != '#')
		إرجاع IdString::Null;
	size_t sep = token.find(':');
	إذا (sep == std::string_view::npos)
		إرجاع IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	لكل (صحيح i = 0; i < 2; i++) {
		إذا (parts[i].empty())
			إرجاع IdString::Null;
		لكل (حرف c : parts[i]) {
			إذا (c < '0' || c > '9')
				إرجاع IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	إذا (fields[0] != serial_)
		إرجاع IdString::Null;
	IdString ref(fields[1]);
	إذا (ref == IdString::Null)
		إرجاع IdString::Null;
	إرجاع is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	فراغ advance(size_t n);
	size_t total_size() ثابت;
خاص:
	فراغ ادفع(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t تعداد_2 = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	إذا (ref == IdString::Null)
		إرجاع;
	لكل (IdString cur = ref.untag(); ;) {
		ثابت Twine_عقدة &t = pool[cur];
		بدل (t.kind()) {
		حالة Twine_عقدة::Kind::Suffix:
			ادفع(t.text());
			cur = t.prefix();
			استمر;
		حالة Twine_عقدة::Kind::Leaf:
			ادفع(t.text());
			اكسر;
		حالة Twine_عقدة::Kind::Dead:
			اكسر;
		}
		اكسر;
	}
	إذا (ref.isPublic())
		ادفع("\\");
	std::reverse(segs(), segs() + تعداد_2);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	ادفع(tail);
}

std::string_view TwineSegments::peek()
{
	بينما (pos < تعداد_2) {
		std::string_view seg = segs()[pos];
		إذا (off < seg.size())
			إرجاع seg.substr(off);
		pos++;
		off = 0;
	}
	إرجاع {};
}

فراغ TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ثابت
{
	size_t total = 0;
	لكل (size_t i = 0; i < تعداد_2; i++)
		total += segs()[i].size();
	إرجاع total;
}

فراغ TwineSegments::ادفع(std::string_view seg)
{
	إذا (spill.empty() && تعداد_2 < SEGMENT_INLINE_DEPTH) {
		inline_segs[تعداد_2++] = seg;
		إرجاع;
	}
	إذا (spill.empty())
		spill.assign(inline_segs, inline_segs + تعداد_2);
	spill.ادفع_back(seg);
	تعداد_2++;
}

std::string_view *TwineSegments::segs() { إرجاع spill.empty() ? inline_segs : spill.data(); }
ثابت std::string_view *TwineSegments::segs() ثابت { إرجاع spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ثابت
{
	إرجاع TwineSegments(*هذا, ref).total_size();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view prefix) ثابت
{
	TwineSegments segs(*هذا, ref);
	بينما (!prefix.empty()) {
		std::string_view seg = segs.peek();
		إذا (seg.empty())
			إرجاع خطأ;
		size_t n = std::min(seg.size(), prefix.size());
		إذا (std::memcmp(seg.data(), prefix.data(), n) != 0)
			إرجاع خطأ;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	إرجاع true;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	إرجاع str_size(ref) == name.size() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	بينما (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		إذا (x.empty() || y.empty())
			إرجاع x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		إذا (صحيح diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			إرجاع diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ثابت {
	إرجاع find_hashed(hash, [&](IdString ref) {
		ثابت Twine_عقدة &t = (*هذا)[ref];
		إذا (t == Twine_عقدة::مفتاح_2{prefix, text})
			إرجاع true;
		IdString head = prefix == Twine_عقدة::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*هذا, ref), sb(*هذا, head, text);
		إرجاع compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ثابت TwinePool& src, IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		إرجاع ref;
	IdString idx = ref.untag();
	ثابت Twine_عقدة& t = src[idx];
	إذا (t.is_dead())
		إرجاع IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		إرجاع compare_segments(sa, sb) == 0;
	});
	إرجاع stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ثابت {
	ثابت Twine_عقدة &t = (*هذا)[ref];
	إذا (!t.is_suffix())
		إرجاع IdString::Null;
	إرجاع stamp(t.prefix().tag(ref.isPublic()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	إذا (a == b)
		إرجاع 0;
	إذا (a == IdString::Null)
		إرجاع -1;
	إذا (b == IdString::Null)
		إرجاع 1;

	إذا (a.isPublic() == b.isPublic()) {
		ثابت Twine_عقدة &ta = (*هذا)[a.untag()];
		ثابت Twine_عقدة &tb = (*هذا)[b.untag()];
		إذا (ta.is_leaf() && tb.is_leaf())
			إرجاع ta.text().compare(tb.text());
		إذا (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			إرجاع ta.text().compare(tb.text());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	إرجاع compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
