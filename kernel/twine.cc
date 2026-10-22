#شامل "kernel/twine.h"
#شامل "kernel/hashcons_impl.h"
#شامل "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template ساختار HashConsPool<TwinePool, Twine_گره, IdString>;

std::vector<Twine_گره> StaticTwines::nodes_;

تهی StaticTwines::init() {
	اگر (ready())
		بازگشت;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	برای (ثابت نویسه *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ثابت Twine_گره &StaticTwines::گره(size_t idx) { بازگشت nodes_[idx]; }
منطقی StaticTwines::ready() { بازگشت nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
صحیح twine_gc_شمار;

std::string IdString::handle_token() ثابت {
	بازگشت stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, منطقی &is_public) {
	اگر (token.substr(0, 5) == "$pub@") {
		is_public = درست;
		بازگشت 5;
	}
	اگر (token.substr(0, 6) == "$priv@") {
		is_public = نادرست;
		بازگشت 6;
	}
	بازگشت 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	بازگشت result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	بازگشت static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

تهی SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	اگر (len_ > INLINE_CAP) {
		ptr_ = جدید نویسه[len_];
		memcpy(ptr_, content.data(), len_);
	} وگرنه {
		memcpy(inl_, content.data(), len_);
	}
}

تهی SmallString::release()
{
	اگر (len_ > INLINE_CAP)
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
	اگر (این != &other)
		store(other.view());
	بازگشت *این;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	اگر (این == &other)
		بازگشت *این;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	بازگشت *این;
}

SmallString::~SmallString() { release(); }

Twine_گره::Twine_گره(Twine_گره &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

Twine_گره &Twine_گره::operator=(Twine_گره &&other) noexcept
{
	اگر (این == &other)
		بازگشت *این;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	بازگشت *این;
}

std::string TwineSpec::content_str() ثابت {
	اگر (auto *leaf = std::get_if<Leaf>(&data))
		بازگشت leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	بازگشت *autosfx.prefix + autosfx.tail;
}

ایستا std::pair<std::string_view, منطقی> unescape_name(std::string_view name) {
	اگر (!name.empty() && name[0] == '\\')
		بازگشت {name.substr(1), درست};
	بازگشت {name, نادرست};
}

ایستا تهی check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	برای (نویسه ch : name)
		اگر ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ثابت TwinePool& other) {
	اگر (این == &other)
		بازگشت *این;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	بازگشت *این;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	اگر (این == &other)
		بازگشت *این;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	بازگشت *این;
}

منطقی TwinePool::owns(IdString ref) ثابت {
	بازگشت ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	اگر (ref == IdString::Null || ID::is_static(ref))
		بازگشت ref;
	بازگشت ref.stamped(serial_);
}

تهی TwinePool::check_owned(IdString ref) ثابت {
	اگر constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_گره& TwinePool::operator[](IdString ref) ثابت {
	check_owned(ref);
	بازگشت HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	برای (auto &it : auto_prefixes)
		roots.درج(it.second);
	بازگشت HashConsPool::gc(roots);
}

ثابت Twine_گره& TwinePool::static_گره(size_t idx) { بازگشت StaticTwines::گره(idx); }
تهی TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	ایستا size_t counter = 0;
	size_t serial = ++counter;
	اگر constexpr (IdString::MAX_SERIAL == 0)
		بازگشت serial;
	وگرنه
		بازگشت serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(Twine_گره t) {
	اگر (free_list.empty() && STATIC_شمار + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_شمار);
	بازگشت HashConsPool::add_inner(std::move(t));
}

تهی TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_گره& twine = (*این)[ref];
	انتخاب (twine.kind()) {
	حالت Twine_گره::Kind::Dead:
		os << "Dead()";
		بشکن;
	حالت Twine_گره::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		بشکن;
	حالت Twine_گره::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		بشکن;
	}
	اگر (ref.isPublic())
		os << " pub";
}

تهی TwinePool::append_str(IdString ref, std::string& out) ثابت {
	اگر (ref == IdString::Null)
		بازگشت;
	اگر (ref.isPublic())
		out += '\\';
	ثابت Twine_گره& twine = (*این)[ref];
	انتخاب (twine.kind()) {
	حالت Twine_گره::Kind::Dead:
		بشکن;
	حالت Twine_گره::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	حالت Twine_گره::Kind::Leaf:
		out += twine.text();
		بشکن;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string out;
	append_str(ref, out);
	بازگشت out;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	بازگشت str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ثابت {
	uint32_t hash = prefix == Twine_گره::NO_PREFIX ? 0 : (*این)[IdString(prefix)].hash();
	بازگشت Twine_گره::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ثابت {
	بازگشت find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	اگر (ref != IdString::Null)
		بازگشت ref;
	بازگشت add_inner(Twine_گره{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ثابت {
	اگر (name.empty())
		بازگشت IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	بازگشت stamp(find_content(Twine_گره::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ثابت {
	اگر (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		بازگشت stamp(find_content(Twine_گره::NO_PREFIX, t.content_str()));
	اگر (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		بازگشت stamp(find_content(Twine_گره::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	بازگشت stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	اگر (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		بازگشت add(auto_prefix(ap->prefix), ap->tail);
	اگر (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		بازگشت stamp(intern(Twine_گره::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	بازگشت add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ثابت std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	اگر (it != auto_prefixes.end())
		بازگشت it->second;
	IdString ref = intern(Twine_گره::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	بازگشت ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	بازگشت stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	اگر (s.empty())
		بازگشت IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	بازگشت stamp(intern(Twine_گره::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	اگر (found != IdString::Null || ref == IdString::Null)
		بازگشت found;
	ثابت Twine_گره& t = src[ref.untag()];
	اگر (t.is_dead())
		بازگشت IdString::Null;
	uint32_t prefix = Twine_گره::NO_PREFIX;
	اگر (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	بازگشت stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	بازگشت "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	اگر (token.size() < 2 || token[0] != '#')
		بازگشت IdString::Null;
	size_t sep = token.find(':');
	اگر (sep == std::string_view::npos)
		بازگشت IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	برای (صحیح i = 0; i < 2; i++) {
		اگر (parts[i].empty())
			بازگشت IdString::Null;
		برای (نویسه c : parts[i]) {
			اگر (c < '0' || c > '9')
				بازگشت IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	اگر (fields[0] != serial_)
		بازگشت IdString::Null;
	IdString ref(fields[1]);
	اگر (ref == IdString::Null)
		بازگشت IdString::Null;
	بازگشت is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ساختار TwineSegments {
	ایستا constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	تهی advance(size_t n);
	size_t total_size() ثابت;
خصوصی:
	تهی افزودن(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t شمار = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	اگر (ref == IdString::Null)
		بازگشت;
	برای (IdString cur = ref.untag(); ;) {
		ثابت Twine_گره &t = pool[cur];
		انتخاب (t.kind()) {
		حالت Twine_گره::Kind::Suffix:
			افزودن(t.text());
			cur = t.prefix();
			ادامه;
		حالت Twine_گره::Kind::Leaf:
			افزودن(t.text());
			بشکن;
		حالت Twine_گره::Kind::Dead:
			بشکن;
		}
		بشکن;
	}
	اگر (ref.isPublic())
		افزودن("\\");
	std::reverse(segs(), segs() + شمار);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	افزودن(tail);
}

std::string_view TwineSegments::peek()
{
	وقتی (pos < شمار) {
		std::string_view seg = segs()[pos];
		اگر (off < seg.size())
			بازگشت seg.substr(off);
		pos++;
		off = 0;
	}
	بازگشت {};
}

تهی TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ثابت
{
	size_t total = 0;
	برای (size_t i = 0; i < شمار; i++)
		total += segs()[i].size();
	بازگشت total;
}

تهی TwineSegments::افزودن(std::string_view seg)
{
	اگر (spill.empty() && شمار < SEGMENT_INLINE_DEPTH) {
		inline_segs[شمار++] = seg;
		بازگشت;
	}
	اگر (spill.empty())
		spill.assign(inline_segs, inline_segs + شمار);
	spill.افزودن_back(seg);
	شمار++;
}

std::string_view *TwineSegments::segs() { بازگشت spill.empty() ? inline_segs : spill.data(); }
ثابت std::string_view *TwineSegments::segs() ثابت { بازگشت spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ثابت
{
	بازگشت TwineSegments(*این, ref).total_size();
}

منطقی TwinePool::begins_with(IdString ref, std::string_view prefix) ثابت
{
	TwineSegments segs(*این, ref);
	وقتی (!prefix.empty()) {
		std::string_view seg = segs.peek();
		اگر (seg.empty())
			بازگشت نادرست;
		size_t n = std::min(seg.size(), prefix.size());
		اگر (std::memcmp(seg.data(), prefix.data(), n) != 0)
			بازگشت نادرست;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	بازگشت درست;
}

منطقی TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	بازگشت str_size(ref) == name.size() && begins_with(ref, name);
}

ایستا صحیح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	وقتی (درست) {
		std::string_view x = sa.peek(), y = sb.peek();
		اگر (x.empty() || y.empty())
			بازگشت x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		اگر (صحیح diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			بازگشت diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ثابت {
	بازگشت find_hashed(hash, [&](IdString ref) {
		ثابت Twine_گره &t = (*این)[ref];
		اگر (t == Twine_گره::کلید_2{prefix, text})
			بازگشت درست;
		IdString head = prefix == Twine_گره::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*این, ref), sb(*این, head, text);
		بازگشت compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ثابت TwinePool& src, IdString ref) ثابت {
	اگر (ref == IdString::Null || ID::is_static(ref))
		بازگشت ref;
	IdString idx = ref.untag();
	ثابت Twine_گره& t = src[idx];
	اگر (t.is_dead())
		بازگشت IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*این, cand), sb(src, idx);
		بازگشت compare_segments(sa, sb) == 0;
	});
	بازگشت stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ثابت {
	ثابت Twine_گره &t = (*این)[ref];
	اگر (!t.is_suffix())
		بازگشت IdString::Null;
	بازگشت stamp(t.prefix().tag(ref.isPublic()));
}

صحیح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	اگر (a == b)
		بازگشت 0;
	اگر (a == IdString::Null)
		بازگشت -1;
	اگر (b == IdString::Null)
		بازگشت 1;

	اگر (a.isPublic() == b.isPublic()) {
		ثابت Twine_گره &ta = (*این)[a.untag()];
		ثابت Twine_گره &tb = (*این)[b.untag()];
		اگر (ta.is_leaf() && tb.is_leaf())
			بازگشت ta.text().compare(tb.text());
		اگر (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			بازگشت ta.text().compare(tb.text());
	}

	TwineSegments sa(*این, a), sb(*این, b);
	بازگشت compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
